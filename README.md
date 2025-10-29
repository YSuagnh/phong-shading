### 图形学第一次实验

实现了 gouraud-shading 与 phong-shading，均使用 blin-phong 光照模型。

#### 使用方法

使用如下指令进行编译:

```bash
mkdir build
cd build
cmake ..
make
```

之后使用 `./opengtest2`运行即可

两个参数第一个表示使用哪种shading，0表示使用 phong-shading ，1表示使用 gouraud-shading

第二个参数表示使用哪种模型，例如 "cube" 或 "dinosaur"