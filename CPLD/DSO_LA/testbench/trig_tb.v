`timescale 1ns/10ps

module fmeter_tb;
    reg clkx;
    reg clks;
    reg clr;
    reg ss;
    wire sta;
    wire ovx;
    wire ovs;
    wire [19:0] cntx;
    wire [19:0] cnts;
    
    // 100MHz
    initial begin
        forever #5 clks = !clks;
    end
    
    // xMHz
    initial begin
        forever #99 clkx = !clkx;
    end

    initial begin
        clkx = 1'b0;
        clks = 1'b0;
        ss = 1'b0;
        #18 clr = 1'b1;
        #18 clr = 1'b0;
        #18 ss = 1'b1;
        #1000 ss = 1'b0;
        #100 $stop;
    end
    
    fmeter fmuut(.fx(clkx), .fs(clks),
                 .clr(clr), .ss(ss), .sta(sta),
                 .ovx(ovx), .ovs(ovs),
                 .cntx(cntx), .cnts(cnts));
endmodule
