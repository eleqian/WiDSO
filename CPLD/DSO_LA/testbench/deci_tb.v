`timescale 1ns/10ps

module deci_tb;
    reg nrst;
    reg clk;
    reg en;
    reg div2;
    reg [15:0] deci_num;
    wire out;
    wire out2;
    
    smpl_deci uut_deci(.nrst(nrst), .clk(clk),
        .en(en), .div2(div2), .num(deci_num), .out(out), .out2(out2));
    
    // 100MHz
    initial begin
        forever #5 clk = ~clk;
    end

    initial begin
        nrst = 1'b0;
        clk = 1'b0;
        en = 1'b0;
        div2 = 1'b0;
        deci_num = 16'h5;
        #20
        nrst = 1'b1;
        $stop;
        #15
        en = 1'b1;
        #200
        div2 = 1'b1;
        #200
        en = 1'b0;
        div2 = 1'b0;
        deci_num = 16'h0;
        #15
        en = 1'b1;
        #100
        div2 = 1'b1;
        #100
        $stop;
    end
endmodule
