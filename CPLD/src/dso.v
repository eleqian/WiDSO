`timescale 1ns/10ps

// 输入/输出控制
module dio_if(oe, dio, din, dout);
    parameter width = 1;
    
    input oe;
    inout [width-1:0] dio;
    input [width-1:0] din;
    output [width-1:0] dout;
    
    assign dout = dio;
    assign dio = oe ? din : {width{1'bz}};
endmodule

// 8分频，可旁路（不分频）
module fdiv8(nrst, clkin, bypass, clkout);
    input nrst;
    input clkin;
    input bypass;
    output clkout;
    
    reg [2:0] cnt;
    
    always @(posedge clkin or negedge nrst) begin
        if (~nrst) begin
            cnt <= 0;
        end else begin
            cnt <= cnt + 1'b1;
        end
    end
    
    assign clkout = bypass ? clkin : cnt[2];
endmodule

/*
功能：DSO
输入：nrst-复位
     clk-时钟
     spi_cs-SPI片选
     spi_sck-SPI时钟
     spi_si-SPI数据输入
     fx_in-待测时钟
     la_in-逻辑分析仪输入
     adc_in-ADC数据输入
输出：spi_so-SPI数据输出
     spi_nirq-SPI中断输出
     adc_clk-ADC时钟输出
     adc_pd-ADC低功耗信号
     fx_out-待测时钟输出
     debug-调试信号输出
*/
module dso(nrst, clk,
           spi_cs, spi_sck, spi_si, spi_so, spi_nirq,
           sram_addr, sram_dio, sram_nce, sram_noe, sram_nwe, sram_nlb, sram_nub,
           adc_in, adc_clk, adc_pd, la_in, fx_in, fx_out, trig_in, trig_out, debug);
    // 全局信号
    input nrst;
    input clk;
    // SPI
    input spi_cs;
    input spi_sck;
    input spi_si;
    output spi_so;
    output spi_nirq;
    // SRAM
    output [17:0] sram_addr;
    inout [15:0] sram_dio;
    output sram_nce;
    output sram_noe;
    output sram_nwe;
    output sram_nlb;
    output sram_nub;
    // ADC
    input [7:0] adc_in;
    output adc_clk;
    output adc_pd;    
    // 其它外设
    input [7:0] la_in;
    input fx_in;
    output fx_out;
    input trig_in;
    output trig_out;
    output debug;
    
    // SPI读写接口
    wire [6:0] s_addr;
    wire [7:0] s_din;
    wire [7:0] s_dout;
    wire spi_so_z;
    wire s_rd;
    wire s_we;
    
    // 公共接口
    wire [7:0] com_ctrl;
    wire [7:0] com_stat;
    
    // 采样抽取接口
    wire [15:0] deci_cnt;
    wire smpl_en;
    
    // 逻辑分析仪接口
    wire la_trig_out;
    wire [7:0] la_in_r;
    
    // 示波器接口
    wire wave_trig_out;
    wire [7:0] adc_in_r;
    
    // 触发器接口
    wire [7:0] trig_ctrl0;
    wire [7:0] trig_ctrl1;
    wire [7:0] trig_ctrl2;

    // SRAM接口
    wire [15:0] sram_din;
    wire [7:0] sram_dat_rd;
    wire [17:0] sram_addr_rd;
    wire [17:0] sram_addr_wr;
    wire [7:0] sram_rd_step;
    wire sram_wr_req;
    wire sram_wr_sta;
    wire sram_rd_sta;
    wire sram_addr_rd_load;
    
    // DSO启动/停止
    wire dso_run;
    // 读取模拟或数字数据
    wire rd_sel;
    // 写SRAM完成标志
    wire dso_wr_end;
    // 触发状态
    wire trig_sta;
    // 输入通道选择
    wire adc_in_sel;
    wire la_in_sel;
    // 输入频率8分频
    wire fx_div8;

    // 公共状态
    assign com_stat = {dso_wr_end, trig_sta};
    // 公共控制
    assign {fx_div8, la_in_sel, adc_in_sel, rd_sel, dso_run} = com_ctrl[4:0];
    
    assign adc_clk = clk;
    assign adc_pd = ~dso_run;
    assign spi_nirq = |com_stat[1:0] ? 1'b0 : 1'bz;
    assign trig_out = trig_sta;

    // SPI从接口
    sspi u_spi(.nrst(nrst), .clk(clk),
        .cs(spi_cs), .sck(spi_sck), .si(spi_si), .so(spi_so_z),
        .addr(s_addr), .din(s_din), .dout(s_dout), .rd(s_rd), .we(s_we));
    assign spi_so = spi_cs ? 1'bz : spi_so_z;
    
    // 地址写译码
    dso_regw u_regw(.nrst(nrst), .clk(clk),
        .addr(s_addr[2:0]), .din(s_dout), .we(s_we),
        .reg0(com_ctrl), .reg1(trig_ctrl0), .reg2(trig_ctrl1), .reg3(trig_ctrl2),
        .reg4(deci_cnt[7:0]), .reg5(deci_cnt[15:8]), .reg6(sram_rd_step));

    // 地址读译码
    dso_regr u_regr(.addr({2'b0, s_addr[0]}), .dout(s_din),
        .reg0(com_stat), .reg1(sram_dat_rd));
    
    // 采样抽取
    smpl_deci u_deci(.nrst(nrst), .clk(clk),
        .en(dso_run), .div2(adc_in_sel ^ la_in_sel), .num(deci_cnt), .out(smpl_en), .out2(sram_wr_req));
    
    // 同步输入数据
    mux21nr #(8) u_muxadc(.nrst(nrst), .clk(clk), .en(smpl_en), .sel(adc_in_sel), 
        .din0(adc_in), .din1(la_in_r), .dout(adc_in_r));
    mux21nr #(8) u_muxla(.nrst(nrst), .clk(clk), .en(smpl_en), .sel(la_in_sel), 
        .din0(la_in), .din1(adc_in_r), .dout(la_in_r));

    // 逻辑触发
    la_trig u_lat(.nrst(nrst), .clk(clk),
        .din(la_in_r), .trig_out(la_trig_out),
        .level_sel(trig_ctrl1), .level_mask(trig_ctrl2),
        .edge_sel(trig_ctrl0[2:0]), .edge_mask(trig_ctrl0[3]));

    // 波形触发
    wave_trig u_wavt(.nrst(nrst), .clk(clk),
        .din(adc_in_r), .level(trig_ctrl1), .edge_sel(trig_ctrl0[4]), .trig_out(wave_trig_out));
    
    // 触发器
    dso_trig u_trig(.nrst(nrst), .clk(clk),
        .wave_trig(wave_trig_out), .wave_bypass(trig_ctrl0[5]), .la_trig(la_trig_out),
        .ext_trig(trig_in), .ext_bypass(trig_ctrl0[6]),
        .trig_clr(trig_ctrl0[7]), .trig_sta(trig_sta), .trig_pluse(sram_addr_rd_load));

    // 读写地址生成
    addr_gen u_addr(.nrst(nrst), .clk(clk),
        .wr_en(dso_run), .wr_inc(sram_wr_req), .load_w2r(sram_addr_rd_load), .rd_inc(s_rd), .rd_step(sram_rd_step), 
        .addr_wr(sram_addr_wr), .addr_rd(sram_addr_rd), .wr_end(dso_wr_end));
    
    // 读写状态控制
    rw_ctrl u_rwc(.nrst(nrst), .clk(clk),
        .wr_req(sram_wr_req), .rd_req(s_rd), .wr_sta(sram_wr_sta), .rd_sta(sram_rd_sta));
    
    // 读写地址选择
    mux21n #(18) u_muxaddr(.sel(sram_wr_sta), .din0(sram_addr_rd), .din1(sram_addr_wr), .dout(sram_addr));
    
    // 读取数据选择
    mux21nr #(8) u_muxdat(.nrst(nrst), .clk(clk), .en(sram_rd_sta), .sel(rd_sel), 
        .din0(sram_din[7:0]), .din1(sram_din[15:8]), .dout(sram_dat_rd));
    
    // SRAM数据io
    dio_if #(16) u_dio(.oe(sram_wr_sta), .dio(sram_dio), .din({la_in_r, adc_in_r}), .dout(sram_din));
    
    // SRAM控制信号
    assign sram_nwe = ~sram_wr_sta;
    assign sram_noe = ~sram_rd_sta;
    assign sram_nce = ~(sram_wr_sta | sram_rd_sta);
    assign sram_nlb = 1'b0;
    assign sram_nub = 1'b0;
    
    // 频率计分频
    fdiv8 u_fxdiv(.nrst(nrst), .clkin(fx_in), .bypass(~fx_div8), .clkout(fx_out));
    
    assign debug = dso_run; //'b0;
endmodule
