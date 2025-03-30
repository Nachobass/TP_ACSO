.text
movz X1, 0x1000
lsl X1, X1, 16     
movz X10, 0x1234     

sturh W10, [X1, 0x0] 
sturh W10, [X1, 0x2] 
ldurh W13, [X1, 0x0] 
ldurh W14, [X1, 0x2] 

HLT 0
