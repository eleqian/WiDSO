`timescale 1ns/10ps

/*
功能：多位计数器
输入：nrst-0异步清除
     clk-时钟
     en-1使能计数
     step-计数步进
     load-使能置数
     cin-置数值
输出：cnt-计数值
*/
module cntn(nrst, clk, en, step, load, cin, cnt);
    parameter width = 8;

    input nrst;
    input clk;
    input en;
    input [width-1:0] step;
    input load;
    input [width-1:0] cin;
    output reg [width-1:0] cnt;
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            cnt <= 0;
        end else if (load) begin
            cnt <= cin;
        end else if (en) begin
            cnt <= cnt + step;
        end
    end
endmodule
