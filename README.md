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


1. 第一个参数选择模型名称，例如 `cube` 或 `dinosaur`


2. 第二个参数选择着色模式：
	- `0`：phong-shading
	- `1`：gouraud-shading
	- `2`：cook-torrance

3. 第三个可选参数选择预定义好几种材质，其中：
	- '0': 黑色金属
	- '1': 铜
	- '2': 银
	- '3': 皮革
	- '4': 铁
	- '5': 碳
	- '6': 金
	- '7': 锡

第二个参数表示使用哪种模型，例如 "cube" 或 "dinosaur"