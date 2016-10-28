`timescale 1ns/10ps

/*
功能：示波器触发器
输入：nrst-0复位
     clk-时钟
     wave_trig-波形触发输入
     wave_bypass-1不关心波形触发
     la_trig-逻辑触发输入
     trig_clr-清除触发状态
输出：trig_sta-触发状态输出，1已触发
     trig_pluse-触发脉冲输出
修改：eleqian 2016-01-20
*/
module dso_trig(nrst, clk, wave_trig, wave_bypass, la_trig, ext_trig, ext_bypass,
                trig_clr, trig_sta, trig_pluse);
    input nrst;
    input clk;
    input wave_trig;
    input wave_bypass;
    input la_trig;
    input ext_trig;
    input ext_bypass;
    input trig_clr;
    output trig_sta;
    output trig_pluse;
    
    reg trig_sta;
    reg trig_last;
    wire trig_cap;
    
    assign trig_cap = (wave_trig | wave_bypass) & (ext_trig | ext_bypass) & la_trig;
    assign trig_pluse = trig_sta & ~trig_last;
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            trig_sta <= 1'b0;
        end else if (trig_clr) begin
            trig_sta <= 1'b0;
        end else if (trig_cap) begin
            trig_sta <= 1'b1;
        end
    end
    
    always @(posedge clk or negedge nrst) begin
        if (~nrst) begin
            trig_last <= 1'b0;
        end else begin
            trig_last <= trig_sta;
        end
    end
endmodule
