# 3D渲染

编辑注意
===
需要先将项目设置为你电脑的WindowsSDK版本。
项目->属性->配置属性->常规->windows SDK版本
此外不需要任何其他配置
===

参考了这个作者的开源项目
===
作者的博客 http://www.skywind.me

与原作不同的地方：
===
* 从C改为了C++
* 尽量靠近Unity的设计，比如transfrom、camera等、顶点排序顺序、X-Y-Z轴朝向
* 增加光照模式：平面着色和高氏着色 、顶点光照、面法线光照
* 增加了摄像机的正面剔除/背面剔除
* 增加了光照阴影

特性
===
* 没有任何第三方库依赖。
* 简单 CVV 裁剪
* 最大支持 1024 x 1024 的纹理
* 使用深度缓存判断图像前后
* 透视纹理映射以及透视色彩填充
* 操作方式：左右键旋转，前后键前进后退，空格键切换模式，ESC退出。

演示
===

纹理填充：RENDER_STATE_TEXTURE
![](https://raw.githubusercontent.com/skywind3000/mini3d/master/images/mini_1.png)

色彩填充：RENDER_STATE_COLOR
![]
(https://github.com/timotimosky/CoolEngine/raw/master/images/render2.png)
线框绘制：RENDER_STATE_WIREFRAME
![](https://raw.githubusercontent.com/skywind3000/mini3d/master/images/mini_2.png)

增加光照和二次线性插值（朋友给 Mini3D 增加的光照效果截图）
![](https://raw.githubusercontent.com/skywind3000/mini3d/master/images/mini_3.png)



基础作业
=======
* 提供更多渲染模式
* 实现二次线性差值的纹理读取

进阶作业
=======
* 优化顶点计算性能
* 优化 draw_scanline 性能
* 从 BMP/TGA 文件加载纹理
* 载入 BSP 场景并实现漫游
* 场景组织、层次细节、动态光照
* 光线跟踪版
