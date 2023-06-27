module led(
  input clk,
  input rst,
  input [9:0] sw,
  output [15:0] ledr
);



always @ (sw)
case(sw[1:0])
0:assign ledr[1:0] = sw[3:2];
1:assign ledr[1:0] = sw[5:4];
2:assign ledr[1:0] = sw[7:6];
3:assign ledr[1:0] = sw[9:8];


endcase


endmodule
