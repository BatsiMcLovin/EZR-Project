# version 450

in vec4 passRadialCurvDeriv;

out vec4 fragcolor;

void main()
{

//fragcolor = vec4(1,0,1,1);

fragcolor = passRadialCurvDeriv;
}
