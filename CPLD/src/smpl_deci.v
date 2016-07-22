`timescale 1ns/10ps

// 采样数据抽取信号，降低实际采样率
module smpl_deci(nrst, clk, en, div2, num, out, out2);
    input nrst;
    input clk;
    input en;
    input div2;
    input [15:0] num;
    output reg out;
    output out2;
    
    wire [15:0] cnt;
    reg outdiv;
    wire cnt_end;
    wire load;
    wire out_sig;
    
    assign cnt_end = (16'b0 == cnt);
    assign load = !en | cnt_end;
    assign out_sig = en & cnt_end;
    
    // 减计数到0
    cntn #(16) u_decicnt(.nrst(nrst), .clk(clk),
        .en(1), .step(-1), .load(load), .cin(num), .cnt(cnt));
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            out <= 0;
        end else begin
            out <= out_sig;
        end
    end
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            outdiv <= 0;
        end else if (out_sig) begin
            outdiv <= ~outdiv;
        end
    end
    
    assign out2 = out & (~outdiv | ~div2);
endmodule
