### 图形学第一次实验

实现了 gouraud-shading 与 phong-shading（Blinn-Phong 模型），并新增了 Cook-Torrance 基于微表面（GGX）PBR 着色模型。

#### 使用方法

使用如下指令进行编译:

```bash
mkdir build
cd build
cmake ..
make
```

之后使用 `./opengltest2` 运行即可

启动参数说明：

1. 第一个参数选择着色模式：
	- `0`：phong-shading（fragment，Blinn-Phong）
	- `1`：gouraud-shading（vertex，Blinn-Phong）
	- `2`：cook-torrance（PBR，GGX/Trowbridge-Reitz + Smith + Schlick Fresnel）

2. 第二个参数选择模型名称，例如 `cube` 或 `dinosaur`


第二个参数表示使用哪种模型，例如 "cube" 或 "dinosaur"