#pragma once

"#ifdef VERTEX\n"
"layout(location=0) in vec2 Position;"
"layout(location=1) in vec2 TexCoord;"
"layout(location=4) in ivec2 TexIds;"
"layout(location=5) in uvec4 Flags;"
"uniform mat4 u_MVP;"
"out vec2 v_TexCoord;"
"flat out ivec2 v_TexIds;"
"flat out uvec4 v_Flags;"
"void main()"
"{"
  "gl_Position=u_MVP*vec4(Position,0,1);"
  "v_TexCoord=TexCoord;"
  "v_TexIds=TexIds;"
  "v_Flags=Flags;"
"}"
"\n#elif FRAGMENT\n"
"layout(location=0) out vec4 FragColor;layout(std140) uniform ubo_Metrics{float u_BloomExp;float u_BloomGamma;vec2 u_RelSize;};"
"uniform sampler2D u_Texture,u_BloomTexture1,u_BloomTexture2;"
"uniform sampler2DArray u_LUTTexture;"
"in vec2 v_TexCoord;"
"flat in ivec2 v_TexIds;"
"flat in uvec4 v_Flags;"
"\n#define CoefLuma vec3(0.2126,0.7152,0.0722)\n"
"vec4 t()"
"{"
  "vec3 u=texture(u_Texture,v_TexCoord).xyz;"
  "float v=dot(u,CoefLuma);"
  "return v>.97?"
    "(u=mix(vec3(dot(u,CoefLuma)),u,50.),vec4(u,1)):"
    "vec4(0);"
"}"
"vec4 t(sampler2D u,int v)"
"{"
  "vec4 i=vec4(0);"
  "vec2 f[13];"
  "if(v==0)"
    "for(int e=-6;e<=6;e++)"
      "f[e+6]=v_TexCoord+vec2(u_RelSize.x*e,0);"
  "else "
     "for(int e=-5;e<=6;e++)"
      "f[e+6]=v_TexCoord+vec2(0,u_RelSize.y*e);"
  "i+=texture(u,f[0])*.000244140625;"
  "i+=texture(u,f[1])*.0029296875;"
  "i+=texture(u,f[2])*.01611328125;"
  "i+=texture(u,f[3])*.0537109375;"
  "i+=texture(u,f[4])*.120849609375;"
  "i+=texture(u,f[5])*.193359375;"
  "i+=texture(u,f[6])*.2255859375;"
  "i+=texture(u,f[7])*.193359375;"
  "i+=texture(u,f[8])*.120849609375;"
  "i+=texture(u,f[9])*.0537109375;"
  "i+=texture(u,f[10])*.01611328125;"
  "i+=texture(u,f[11])*.0029296875;"
  "i+=texture(u,f[12])*.000244140625;"
  "return i;"
"}"
"vec4 t(vec4 u,vec4 v,float f)"
"{"
  "return u.z<1.?"
    "mix(u,v,f):"
    "u;"
"}"
"\n#define LUT_Size 32.0\n"
"vec4 t(vec4 u)"
"{"
  "float v=(u.x*(LUT_Size-1.)+.4999)/(LUT_Size*LUT_Size),f=(u.y*(LUT_Size-1.)+.4999)/LUT_Size,e=floor(u.z*(LUT_Size-1.))/LUT_Size+v,i=ceil(u.z+1e-6*(LUT_Size-1.))/LUT_Size+v,n=clamp(max((u.z-e)/(i-e),0.),0.,32.);"
  "vec4 s=texture(u_LUTTexture,vec3(vec2(e,f),v_TexIds.x-1)),r=texture(u_LUTTexture,vec3(vec2(i,f),v_TexIds.x-1));"
  "return t(s,r,n);"
"}"
"void main()"
"{"
  "switch(v_Flags.x){"
    "case 0u:"
      "FragColor=t();"
      "break;"
    "case 1u:"
      "FragColor=t(u_BloomTexture2,0);"
      "break;"
    "case 2u:"
      "FragColor=t(u_BloomTexture2,1);"
      "break;"
    "case 3u:"
    "case 4u:"
      "vec3 u=texture(u_Texture,v_TexCoord).xyz;"
      "if(v_Flags.x==4u)"
        "{"
          "vec3 v=texture(u_BloomTexture1,v_TexCoord).xyz;"
          "v=vec3(1)-exp(-v*u_BloomExp);"
          "v=pow(v,vec3(1./u_BloomGamma));"
          "u=clamp(u+v,0.,1.);"
        "}"
      "FragColor=vec4(u,1);"
      "FragColor=v_TexIds.x>0?"
        "t(FragColor):"
        "FragColor;"
      "break;"
  "}"
"}"
"\n#elif COMPUTE\n"
"layout(local_size_x=16,local_size_y=16)in;"
"uniform sampler2D u_InTexture;"
"writeonly uniform image2D u_OutTexture;"
"uniform uint u_Flag=0;"
"vec4 e(sampler2D u,ivec2 v,int i)"
"{"
  "vec4 e=vec4(0);"
  "ivec2 f=textureSize(u,0)-1,C[17];"
  "if(i==0)"
    "for(int r=-8;r<=8;r++)"
      "C[r+8]=ivec2(clamp(v.x+r,1,f.x),v.y);"
  "else "
     "for(int r=-8;r<=8;r++)"
      "C[r+8]=ivec2(v.x,clamp(v.y+r,1,f.y));"
  "e+=texelFetch(u,C[0],0)*.0000777505714898;"
  "e+=texelFetch(u,C[1],0)*.0004886523773991;"
  "e+=texelFetch(u,C[2],0)*.002402783593204;"
  "e+=texelFetch(u,C[3],0)*.0092448129378371;"
  "e+=texelFetch(u,C[4],0)*.0278352765221734;"
  "e+=texelFetch(u,C[5],0)*.0655921303317756;"
  "e+=texelFetch(u,C[6],0)*.120980030195482;"
  "e+=texelFetch(u,C[7],0)*.174670181278773;"
  "e+=texelFetch(u,C[8],0)*.197416764383733;"
  "e+=texelFetch(u,C[9],0)*.174670181278773;"
  "e+=texelFetch(u,C[10],0)*.120980030195482;"
  "e+=texelFetch(u,C[11],0)*.0655921303317756;"
  "e+=texelFetch(u,C[12],0)*.0278352765221734;"
  "e+=texelFetch(u,C[13],0)*.0092448129378371;"
  "e+=texelFetch(u,C[14],0)*.002402783593204;"
  "e+=texelFetch(u,C[15],0)*.0004886523773991;"
  "e+=texelFetch(u,C[16],0)*.0000777505714898;"
  "return e;"
"}"
"void main()"
"{"
  "ivec2 u=ivec2(gl_GlobalInvocationID.xy);"
  "vec4 v=vec4(0);"
  "switch(u_Flag){"
    "case 0u:"
      "v=e(u_InTexture,u,0);"
      "break;"
    "case 1u:"
      "v=e(u_InTexture,u,1);"
      "break;"
  "}"
  "imageStore(u_OutTexture,u,v);"
"}"
"\n#endif"