`timescale 1ns/10ps

/*
功能：生成SPI控制信号
输入：nrst, clk, bitcnt
输出：sig_last, sig_tc
*/
module sspi_sig(nrst, clk, bitcnt, sig_last, sig_tc);
    input nrst;
    input clk;
    input [2:0] bitcnt;
    output sig_last;
    output sig_tc;
    
    reg bits7_r;
    wire bits7;
    
    assign bits7 = (bitcnt == 3'b111);
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            bits7_r <= 1'b0;
        end else begin
            bits7_r <= bits7;
        end
    end
    
    assign sig_tc = ({bits7_r, bits7} == 2'b10);
    assign sig_last = ({bits7_r, bits7} == 2'b01);
endmodule

/*
功能：同步SPI输入信号
输入：nrst, clk, cs, sck, si
输出：cs_s, sck_p, sck_n, si_s
*/
module sspi_sync(nrst, clk, cs, sck, si, cs_s, sck_p, sck_n, si_s);
    input nrst;
    input clk;
    input cs;
    input sck;
    input si;
    output cs_s;
    output sck_p;
    output sck_n;
    output si_s;
    reg [1:0] cs_r;
    reg [1:0] si_r;
    reg [2:0] sck_r;
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            sck_r <= 3'b111;
            cs_r <= 2'b11;
            si_r <= 2'b00;
        end else begin
            sck_r <= {sck_r[1:0], sck};
            cs_r <= {cs_r[0], cs};
            si_r <= {si_r[0], si};
        end
    end
    
    assign sck_p = (sck_r[2:1] == 2'b01);
    assign sck_n = (sck_r[2:1] == 2'b10);
    assign cs_s = cs_r[1];
    assign si_s = si_r[1];
endmodule

/*
功能：串并转换
输入：nrst-0复位
     clk-时钟
     en-1使能
     si-串行输入
     load-1并行加载
     pi-并行输入
输出：po-并行输出
*/
module s2p(nrst, clk, en, si, load, pi, po);
    parameter width = 8;
    input nrst;
    input clk;
    input en;
    input si;
    input load;
    input [width-1:0] pi;
    output [width-1:0] po;
    reg [width-1:0] po;
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            po <= 0;
        end else if (load) begin
            po <= pi;
        end else if (en) begin
            po <= {po[width-2:0], si};
        end
    end
endmodule

/*
功能：SPI从接口
极性：SPI模式3，SCLK空闲状态为高电平，在上升沿采样（第二个边沿）
协议：首字节bit[7:1]为地址，bit[0]为读(0)/写(1)，后续字节为数据
     读格式：MOSI: 地址[7]|0[1]|dummy[n*8]
            MISO: 0[8]|读出的数据[n*8]
     写格式：MOSI: 地址[7]|1[1]|要写入的数据[n*8]
            MISO: 0[8]|原始值[8]|被写入的数据[(n-1)*8]
输入：nrst, clk, cs, sck, si, din
输出：so, addr, dout, rd, we
修改：eleqian 2016-01-9
*/
module sspi(nrst, clk, cs, sck, si, so, addr, din, dout, rd, we);
    input nrst;
    input clk;
    input cs;
    input sck;
    input si;
    output so;
    output [6:0] addr;
    input [7:0] din;
    output [7:0] dout;
    output rd;
    output we;
    
    //reg so;
    reg [6:0] addr;
    reg d_na;   // 1-dat/0-addr
    reg w_nr;   // 1-write/0-read
    wire cs_s;
    wire sck_p;
    wire sck_n;
    wire si_s;
    wire [7:0] data;
    wire [2:0] bitcnt;
    wire sig_tc;
    wire sig_last;
    
    // 同步输入信号
    sspi_sync u_sync(.nrst(nrst), .clk(clk), .cs(cs), .sck(sck), .si(si),
                       .cs_s(cs_s), .sck_p(sck_p), .sck_n(sck_n), .si_s(si_s));
    
    // bit计数
    cntn #(3) u_cnt(.nrst(nrst), .clk(clk), .en(sck_p), .step(1), .load(cs_s), .cin(0), .cnt(bitcnt));
    
    // 产生传输完成信号
    sspi_sig u_sig(.nrst(nrst), .clk(clk), .bitcnt(bitcnt), .sig_last(sig_last), .sig_tc(sig_tc));
    
    // 产生数据读写信号
    assign we = w_nr && sig_tc;
    assign rd = ~w_nr && sig_tc;

    // 接收数据
    s2p u_s2p(.nrst(nrst), .clk(clk), .en(sck_p), .si(si_s),
                .load(rd), .pi(din), .po(data));
    assign dout = data;

    // 发送数据
    /*always @(negedge sck or negedge nrst) begin
        if (~nrst) begin
            so <= 1'b0;
        end else begin
            so <= data[7];
        end
    end*/
    // 为提高sck速率，miso不由sck下降沿同步，否则可能建立时间不满足
    // 因为对sck同步已延迟2~3个clk，通常满足miso保持时间
    assign so = data[7];
    
    // 地址/数据状态转换
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            d_na <= 1'b0;
        end else if (cs_s) begin
            d_na <= 1'b0;
        end else if (sig_tc) begin
            d_na <= 1'b1;
        end
    end

    // 接收读写标志
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            w_nr <= 1'b0;
        end else if (cs_s) begin
            w_nr <= 1'b0;
        end else if (~d_na && sig_tc) begin
            w_nr <= data[0];
        end
    end

    // 接收地址
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            addr <= 7'b0;
        end else if (~d_na && sig_last) begin
            addr <= data[6:0];
        end
    end
endmodule
