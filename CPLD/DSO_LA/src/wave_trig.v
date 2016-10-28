`timescale 1ns/10ps

/*
功能：波形触发器
输入：nrst-0复位
     clk-时钟
     din-数据输入
     level-触发电平
     edge_sel-触发边沿选择，0下降沿，1上升沿
输出：trig_out-1成功触发
修改：eleqian 2016-01-20
*/
module wave_trig(nrst, clk, din, level, edge_sel, trig_out);
    input nrst;
    input clk;
    input [7:0] din;
    input [7:0] level;
    input edge_sel;
    output trig_out;
    
    reg trig_out;
    wire edge_cap;
    
    assign edge_cap = edge_sel ^ (din < level);
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            trig_out <= 1'b0;
        end else if (trig_out ^ edge_cap) begin
            trig_out <= edge_cap;
        end
    end
endmodule
