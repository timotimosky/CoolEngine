# 3D渲染

编辑注意
===
需要先将项目设置为你电脑的WindowsSDK版本，不然会报错。
项目->属性->配置属性->常规->windows SDK版本
此外不需要任何其他配置
===

特性
===
* 继承的一个开源项目，原作者的博客 http://www.skywind.me
* 没有任何第三方库依赖。
* 标准 D3D 坐标模型，左手系加 WORLD / VIEW / PROJECTION 三矩阵
* 简单 CVV 裁剪
* 最大支持 1024 x 1024 的纹理
* 使用深度缓存判断图像前后
* 透视纹理映射以及透视色彩填充
* 精确的多边形边缘覆盖计算
* 操作方式：左右键旋转，前后键前进后退，空格键切换模式，ESC退出。

与原作不同的地方：
===
* 从C改为了C++
* 重构了整体框架，更适合扩展
* 增加两种光照模式：平面着色和高氏着色. 
* 增加了摄像机的正面剔除/背面剔除

演示
===

纹理填充：RENDER_STATE_TEXTURE
![](https://raw.githubusercontent.com/skywind3000/mini3d/master/images/mini_1.png)

色彩填充：RENDER_STATE_COLOR
![](https://raw.githubusercontent.com/skywind3000/mini3d/master/images/mini_0.png)

线框绘制：RENDER_STATE_WIREFRAME
![](https://raw.githubusercontent.com/skywind3000/mini3d/master/images/mini_2.png)

增加光照和二次线性插值（朋友给 Mini3D 增加的光照效果截图）
![](https://raw.githubusercontent.com/skywind3000/mini3d/master/images/mini_3.png)


transform：实现坐标变换，和书本手册同
vertex： 如何定义顶点？如何定义边？如何定义扫描线？如何定义渲染主体（trapezoid）？
device： 设备，如何 projection，如何裁剪和归一化，如何切分三角形，如何顶点排序？
trapezoid：如何生成 trape，如何生成边，如何计算步长，如何计算扫描线
scanline：如何绘制扫描线，如何透视纠正，如何使用深度缓存，如何绘制


基础作业
=======
* 提供更多渲染模式
* 实现二次线性差值的纹理读取

进阶作业
=======
* 推导并证明程序中用到的所有几何知识
* 优化顶点计算性能
* 优化 draw_scanline 性能
* 从 BMP/TGA 文件加载纹理
* 载入 BSP 场景并实现漫游


* 场景组织、层次细节、动态光照
* 光线跟踪版
