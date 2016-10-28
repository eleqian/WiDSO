`timescale 1ns/10ps

module rw_tb;
    reg nrst;
    reg clk;
    reg wr_req;
    reg rd_req;
    wire wr_sta;
    wire rd_sta;
    
    rw_ctrl uut_rw(.nrst(nrst), .clk(clk),
        .wr_req(wr_req), .rd_req(rd_req), .wr_sta(wr_sta), .rd_sta(rd_sta));
    
    // 100MHz
    initial begin
        forever #5 clk = ~clk;
    end

    initial begin
        nrst = 1'b0;
        clk = 1'b0;
        wr_req = 1'b0;
        rd_req = 1'b0;
        #20
        
        nrst = 1'b1;
        $stop;
        #20
        
        wr_req = 1'b1;
        #10
        wr_req = 1'b0;
        #50
        
        rd_req = 1'b1;
        #10
        rd_req = 1'b0;
        #50
        
        wr_req = 1'b1;
        rd_req = 1'b1;
        #10
        wr_req = 1'b0;
        rd_req = 1'b0;
        #50
        
        wr_req = 1'b1;
        #10
        wr_req = 1'b0;
        rd_req = 1'b1;
        #10
        rd_req = 1'b0;
        #50
        
        rd_req = 1'b1;
        #10
        rd_req = 1'b0;
        wr_req = 1'b1;
        #10
        wr_req = 1'b0;
        #50
        
        $stop;
    end
endmodule
