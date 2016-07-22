`timescale 1ns/10ps

/*
功能：读写请求控制，写优先级高于读
输入：nrst-0复位
     clk-时钟
     wr_req-写请求
     rd_req-读请求
输出：wr_sta-写状态，1可写
     rd_sta-读状态，1可读
修改：eleqian 2016-01-22
*/
module rw_ctrl(nrst, clk, wr_req, rd_req, wr_sta, rd_sta);
    input nrst;
    input clk;
    input wr_req;
    input rd_req;
    output wr_sta;    // 处于写状态
    output rd_sta;    // 处于读状态
    
    reg wr_sta; 
    reg rd_pend;    // 读请求pending信号
    
    // 写状态转换
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            wr_sta <= 1'b0;
        end else begin
            wr_sta <= wr_req;
        end
    end
    
    // 读pending状态转换
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            rd_pend <= 1'b0;
        end else if (rd_req) begin
            rd_pend <= 1'b1;
        end else if (rd_sta) begin
            rd_pend <= 1'b0;
        end
    end
    
    // 当写状态无效且读pending时才可读
    assign rd_sta = rd_pend & ~wr_sta;
endmodule
