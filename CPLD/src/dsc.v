
module dsc(nrst, clk,
           spi_cs, spi_sck, spi_si, spi_so,
           sram_addr, sram_dio, sram_nce, sram_noe, sram_nwe, sram_nlb, sram_nub,
           ch1d, ch2d, misc, nirq, led);
    // 全局信号
    input nrst;
    input clk;
    // SPI
    input spi_cs;
    input spi_sck;
    input spi_si;
    output spi_so;
    // SRAM
    output [18:0] sram_addr;
    inout [15:0] sram_dio;
    output sram_nce;
    output sram_noe;
    output sram_nwe;
    output sram_nlb;
    output sram_nub;
    // 扩展口
    inout [11:0] ch1d;
    inout [11:0] ch2d;
    // 其它
    inout [2:0] misc;
    output [1:0] nirq;
    output led;
    
    // 扩展口功能映射
    wire [7:0] adc_in;
    wire adc_clk;
    wire adc_pd;    
    wire fx_in;
    wire fx_out;
    wire [7:0] la_in;
    wire trig_in;
    wire trig_out;
    
    assign fx_in = ch1d[0];
    assign ch1d[0] = 1'bz;
    assign ch1d[2] = adc_pd;
    assign ch1d[3] = adc_clk;
    assign adc_in[7:0] = ch1d[11:4];
    assign ch1d[11:4] = 8'bz;

    assign la_in[7:0] = ch2d[7:0];
    assign trig_in = ch2d[8];
    assign ch2d[8:0] = 9'bz;
    assign ch2d[9] = trig_out;
    
    assign misc[0] = fx_out;
    assign sram_addr[18] = 0;

    dso uut(.nrst(nrst), .clk(clk),
            .spi_cs(spi_cs), .spi_sck(spi_sck), .spi_si(spi_si), .spi_so(spi_so), .spi_nirq(nirq[0]),
            .sram_addr(sram_addr[17:0]), .sram_dio(sram_dio), .sram_nce(sram_nce), .sram_noe(sram_noe),
            .sram_nwe(sram_nwe), .sram_nlb(sram_nlb), .sram_nub(sram_nub),
            .adc_in(adc_in), .adc_clk(adc_clk), .adc_pd(adc_pd),
            .la_in(la_in), .fx_in(fx_in), .fx_out(fx_out), .trig_in(trig_in), .trig_out(trig_out), .led(led));
endmodule
