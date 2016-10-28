`timescale 1ns/10ps

/*
功能：逻辑分析仪触发器
输入：nrst-0复位
     clk-时钟
     din-数据输入
     level_sel-触发电平选择，0低电平，1高电平
     level_mask-电平触发掩码，1为不关心
     edge_sel-边沿触发通道选择,0~7
     edge_mask-边沿触发掩码，1为不关心
输出：trig_out-1成功触发
修改：eleqian 2016-01-22
*/
module la_trig(nrst, clk, din, level_sel, level_mask, edge_sel, edge_mask, trig_out);
    input nrst;
    input clk;
    input [7:0] din;
    input [7:0] level_sel;
    input [7:0] level_mask;
    input [2:0] edge_sel;
    input edge_mask;
    output trig_out;
    
    reg d_last;
    wire d_sel;
    wire d_edge;
    wire m_edge;
    
    wire [7:0] d_level;
    wire [7:0] m_level;
    wire s_level;
    
    // 选择单个信号检测边沿
    assign d_sel = din[edge_sel];
    
    // 延迟一个clk以检测边沿
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            d_last <= 1'b0;
        end else begin
            d_last <= d_sel;
        end
    end
    
    // 边沿检测
    assign d_edge = d_last ^ d_sel;
    // 边沿掩码
    assign m_edge = d_edge | edge_mask;
    // 电平选择
    assign d_level = ~(din ^ level_sel);
    // 电平掩码
    assign m_level = d_level | level_mask;
    // 各电平为与
    assign trig_out = &m_level & m_edge;
endmodule
