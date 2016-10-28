`timescale 1ns/10ps

// 38译码
module decode38(en, din, dout);
    input en;
    input [2:0] din;
    output reg [7:0] dout;
    
    always @(*) begin
        if (~en) begin
            dout <= 8'b0;
        end else begin
            dout <= 8'b1 << din;
        end
    end
endmodule

// 单个寄存器(x bits)
module regxb(nrst, clk, en, din, regout);
    parameter bits = 1;
    input nrst;
    input clk;
    input en;
    input [bits-1:0] din;
    output reg [bits-1:0] regout;
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            regout <= 0;
        end else if (en) begin
            regout <= din;
        end
    end
endmodule

// 写寄存器
module dso_regw(nrst, clk, addr, din, we, sel,
    reg0, reg1, reg2, reg3, reg4, reg5, reg6, reg7);
    
    input nrst;
    input clk;
    input [2:0] addr;
    input [7:0] din;
    input we;
    output [7:0] sel;
    output [7:0] reg0;
    output [7:0] reg1;
    output [7:0] reg2;
    output [7:0] reg3;
    output [7:0] reg4;
    output [7:0] reg5;
    output [7:0] reg6;
    output [7:0] reg7;
    wire [7:0] addr_dec;
    
    decode38 dec(.en(we), .din(addr), .dout(addr_dec));
    assign sel = addr_dec;
    
    regxb #(8) r0(.nrst(nrst), .clk(clk), .en(addr_dec[0]), .din(din), .regout(reg0));
    regxb #(8) r1(.nrst(nrst), .clk(clk), .en(addr_dec[1]), .din(din), .regout(reg1));
    regxb #(8) r2(.nrst(nrst), .clk(clk), .en(addr_dec[2]), .din(din), .regout(reg2));
    regxb #(8) r3(.nrst(nrst), .clk(clk), .en(addr_dec[3]), .din(din), .regout(reg3));
    regxb #(8) r4(.nrst(nrst), .clk(clk), .en(addr_dec[4]), .din(din), .regout(reg4));
    regxb #(8) r5(.nrst(nrst), .clk(clk), .en(addr_dec[5]), .din(din), .regout(reg5));
    regxb #(8) r6(.nrst(nrst), .clk(clk), .en(addr_dec[6]), .din(din), .regout(reg6));
    regxb #(8) r7(.nrst(nrst), .clk(clk), .en(addr_dec[7]), .din(din), .regout(reg7));
endmodule

// 读寄存器
module dso_regr(addr, dout, 
    reg0, reg1, reg2, reg3, reg4, reg5, reg6, reg7);
    
    input [2:0] addr;
    output reg [7:0] dout;
    input [7:0] reg0;
    input [7:0] reg1;
    input [7:0] reg2;
    input [7:0] reg3;
    input [7:0] reg4;
    input [7:0] reg5;
    input [7:0] reg6;
    input [7:0] reg7;
    
    always @(*) begin
        case (addr)
        3'h0: dout <= reg0;
        3'h1: dout <= reg1;
        3'h2: dout <= reg2;
        3'h3: dout <= reg3;
        3'h4: dout <= reg4;
        3'h5: dout <= reg5;
        3'h6: dout <= reg6;
        3'h7: dout <= reg7;
        default: dout <= 8'b0;
        endcase
    end
endmodule
