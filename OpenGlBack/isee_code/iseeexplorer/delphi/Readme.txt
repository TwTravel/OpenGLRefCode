
                  ISee 图像读写插件 Delphi 移植包
                  ===============================
                      与月共舞(yygw@163.com) 
                            2002.07.01


该目录下的文件为 ISee 图像读写插件的 Delphi 移植开发包。
该开发包包含以下一些内容：

  * ISee 图像读写插件接口 C 头文件的 Delphi 声明文件：
      iseeirw.pas	- 图像读写模块全局定义文件
      iseeio.pas	- ISee I/O 定义文件
      ce.pas		- ISee 图像转换引擎

  * 在 Delphi/BCB/Kylix 中产生编译指令和编译器版本信息的包含文件：
      isee.inc		- 编译指令包含文件
      jedi.inc		- 编译器版本信息包含文件(MPL协议)

  * ISee 图像读写封装类和组件单元：
      ISee.pas		- 基于插件的图像读写引擎封装单元
      ISeeCtrl.pas	- 与插件相关的组件实现单元
      ISeeCtrl.dcr	- 组件图标文件（由 CnPack 开发组成员
                          Passion(shanzhashu@163.com) 制作）

  * doc 开发包文档目录：
      Readme.txt	- 本说明文档
      doc/ISee.chm	- 开发包源码帮助文档
      doc/ISeeImage_UML.wmf	- ISee 图像类 UML 类结构图
      doc/ISeePlugin_UML.wmf	- ISee 插件类 UML 类结构图
      doc/ISeeCtrls_UML.wmf	- ISee 组件 UML 类结构图

  * model 对象模型文件
      model/ISee.mpb	- 使用 ModelMaker6 创建的对象模型文件，包含了
                          ISee.pas 和 ISeeCtrls.pas 单元的实现内容

  * demo 开发包演示程序
      demo/*.*		- 使用该开发包编写的演示程序源码
      注：该演示程序使用了RxLib2.75控件包，编译时请注意。
          演示程序运行时需要用到ce.dll、iseeio.dll、iseeirw.dll和一些VC调试库。


要获得该开发包的最新版本及相关内容，请访问 ISee 开发网站：

http://isee.126.com

