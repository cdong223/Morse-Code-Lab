
char table[16][5] = {"DDDDD",
                    "dDDDD",
                    "ddDDD",
                    "dddDD",
                    "ddddD",
                    "ddddd",
                    "Ddddd",
                    "DDddd",
                    "DDDdd",
                    "DDDDd",
                    "dD000",
                    "Dddd0",
                    "DdDd0",
                    "Ddd00",
                    "d0000",
                    "ddDd0"
};

void main(void){
  printf("Two Dimensional array elements:\n");
  for(i=0; i<16; i++) {
    for(j=0;j<5;j++) {
      printf("%d ", disp[i][j]);
      if(j==4){
        printf("\n");
      }
    }
  }
}
