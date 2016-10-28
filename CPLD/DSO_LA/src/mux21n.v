`timescale 1ns/10ps

// 多路2选1
module mux21n(sel, din0, din1, dout);
    parameter width = 1;
    input sel;
    input [width-1:0] din0;
    input [width-1:0] din1;
    output [width-1:0] dout;
    
    assign dout = (0 == sel) ? din0 : din1;
endmodule

// 带输出寄存器的多路2选1
module mux21nr(nrst, clk, en, sel, din0, din1, dout);
    parameter width = 1;
    input nrst;
    input clk;
    input en;
    input sel;
    input [width-1:0] din0;
    input [width-1:0] din1;
    output reg [width-1:0] dout;
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            dout <= 0;
        end else if (en) begin
            dout <= (0 == sel) ? din0 : din1;
        end
    end
endmodule
