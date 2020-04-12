#include <stdio.h>
int main(){
float a = 1.0f;
printf("%08x",*(int*)&a);
return 0;
}
