`timescale 1ns/10ps

module sspi_tb;
    reg nrst;
    reg clk;
    reg cs;
    reg sck;
    reg si;
    wire so;
    wire [6:0] addr;
    reg [7:0] din;
    wire [7:0] dout;
    wire rd;
    wire we;
    integer i;
    integer j;
    reg [7:0] spi_send;
    reg [7:0] spi_recv;
    reg [7:0] data_rw;

    sspi uut0(.nrst(nrst), .clk(clk), .cs(cs), .sck(sck), .si(si), .so(so),
                  .addr(addr), .din(din), .dout(dout), .rd(rd), .we(we));	

    task gensck;
        for (i = 0; i < 8; i = i + 1) begin
            #28
            sck = 0;
            j = i;
            #28
            sck = 1;
        end
    endtask
    
    initial begin
        i = 0;
        j = 0;
        clk = 0;
        nrst = 1;
        cs = 0;
        sck = 1;
        si = 0;
        din = 8'h00;
        data_rw = 8'h0;
        spi_send = 8'h0;
        spi_recv = 8'h0;
        #10
        nrst = 0;
        #10
        nrst = 1;
        cs = 1;
        
        $stop;
        
        #20
        cs = 0;
        #100
        spi_send = {7'd1, 1'b0};
        spi_recv = 8'h0;
        gensck();
        spi_send = 8'h3e;
        spi_recv = 8'h0;
        gensck();
        #100
        spi_recv = 8'h0;
        gensck();
        #100
        cs = 1;
        #100
        gensck();
        
        #20
        cs = 0;
        #100
        spi_send = {7'd2, 1'b1};
        spi_recv = 8'h0;
        gensck();
        #100
        spi_send = 8'hc7;
        gensck();
        #100
        spi_send = 8'h4d;
        gensck();
        #100
        cs = 1;
        #100
        gensck();
        
        #20 $stop;
    end

    initial begin
       forever #5 clk = !clk;
    end

    always @(negedge sck) begin
        si <= spi_send[7 - i];
    end

    always @(posedge sck) begin
        spi_recv[7 - j] <= so;                                       
    end
    
    always @(*) begin
        case (addr[1:0])
        'h0:
            din <= 8'h5a;
        'h1:
            din <= 8'hcc;
        'h2:
            din <= 8'h38;
        'h3:
            din <= 8'h67;
         default:
            din <= 8'hzz;
         endcase
    end
    
    always @(posedge clk) begin
        if (we) begin
            data_rw <= dout;
        end
    end
endmodule
