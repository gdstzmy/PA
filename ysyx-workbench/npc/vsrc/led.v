module led(
  input clk,
  input rst,
  input [9:0] sw,
  output [15:0] ledr
);



always @ (sw)
case(sw[1:0])
0:assign ledr[0] = 1;
1:assign ledr[1] = 1;
2:assign ledr[2] = 1;
3:assign ledr[3] = 1;


endcase


endmodule
