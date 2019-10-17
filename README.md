# 3D渲染

参考
===
1.韦易笑的博客 http://www.skywind.me
2.国外一个开源的渲染器教程
https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling


特性
===
* C++边写,没有任何第三方库依赖,使用模板编写数学库
* 支持加载Obj、支持加载Png纹理
* 尽量靠近Unity的设计，增加了object、transfrom、camera等、顶点排序顺序、X-Y-Z轴朝向
* 支持多相机
* 增加了多个物体同时渲染 :支持多个默认形状：平面/立方体/球形
* 增加光照模式：平面着色和高氏着色 、顶点光照、面法线光照
* 增加了摄像机的正面剔除/背面剔除
* 增加了点光源阴影
* 没有任何第三方库依赖。
* 简单 CVV 裁剪
* 最大支持 1024 x 1024 的纹理
* 使用深度缓存判断图像前后
* 透视纹理映射以及透视色彩填充
* 操作方式：左右键旋转，前后键前进后退，空格键切换模式，ESC退出。

演示
===
色彩填充：RENDER_STATE_COLOR
![]
(https://raw.githubusercontent.com/timotimosky/CoolEngine/master/images/render1.png)


后续计划
=======
* 提供更多渲染模式
* 实现二次线性差值的纹理读取
* 从jpg、bmp/TGA文件加载纹理
* 附带光线跟踪
* 优化顶点计算性能
* 优化 draw_scanline 性能
* 载入 BSP 场景并实现漫游
* 场景组织、层次细节、动态光照
