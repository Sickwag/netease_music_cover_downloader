# 🎵 网易云音乐封面下载器 (NetEase Music Cover Downloader)

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Windows-blue.svg" alt="Platform">
  <img src="https://img.shields.io/badge/Language-C++-orange.svg" alt="Language">
  <img src="https://img.shields.io/badge/Build-CMake-green.svg" alt="Build">
  <img src="https://img.shields.io/badge/License-MIT-blue.svg" alt="License">
</p>

<p align="center">
  <img src="https://user-images.githubusercontent.com/12345678/123456789-1234567890abcdef.png" alt="Preview" width="400">
</p>

## 🌟 项目介绍

这是一个用C++编写的网易云音乐封面下载器，可以帮助你轻松批量下载喜欢的歌曲和专辑封面。无论是想收藏精美封面，还是需要为音乐库整理图片，这个小工具都能帮你搞定！

> **"让每一张封面都成为你音乐回忆的一部分"** 🎨

### 🎯 核心特性

- 🚀 **批量下载** - 一次输入多个歌曲/专辑链接，批量下载封面
- 🎨 **智能解析** - 自动识别歌曲名、歌手名，精准提取封面
- 📝 **多样命名** - 支持4种文件命名格式，满足不同需求
- 🌐 **完美中文** - 完美支持中文文件名，告别乱码困扰
- 🛡️ **安全可靠** - 模拟浏览器请求，智能重试机制
- ⚡ **高速下载** - 多线程优化，下载速度快如闪电

## 🎯 使用场景

- 📚 **音乐收藏家** - 收集喜欢歌曲的精美封面
- 🎵 **音乐博主** - 快速获取高质量专辑封面
- 🎨 **设计师** - 获取灵感素材
- 📖 **个人整理** - 整理本地音乐库封面

## 🚀 快速开始

### 📦 环境要求

- Windows 7/8/10/11
- CMake 3.20+
- Boost库 (system组件)
- Visual Studio 2019+ 或 MinGW

### 🔧 编译安装

```bash
# 克隆项目
git clone https://github.com/yourusername/netease_music_cover_downloader.git
cd netease_music_cover_downloader

# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译项目
cmake --build . --config Release
```

### 🎵 使用方法

1. **运行程序**
   ```bash
   cd build/Release
   ./netease_music_cover.exe
   ```

2. **输入链接**
   ```
   https://music.163.com/#/song?id=123456
   https://music.163.com/#/album?id=789012
   ```

3. **选择命名格式**
   ```
   请选择封面命名格式：
   1. 歌手 - 歌名
   2. 歌名 - 歌手
   3. 序号.歌手 - 歌名
   4. 序号.歌名 - 歌手
   ```

4. **坐等下载完成！** 🎉

### 📁 输出示例

```
covers/
├── 周杰伦 - 青花瓷.jpg
├── 01.陈奕迅 - 十年.jpg
└── 02.林俊杰 - 江南.jpg
```

## 🎨 命名格式预览

| 格式 | 示例 |
|------|------|
| 1. 歌手 - 歌名 | `周杰伦 - 青花瓷.jpg` |
| 2. 歌名 - 歌手 | `青花瓷 - 周杰伦.jpg` |
| 3. 序号.歌手 - 歌名 | `01.周杰伦 - 青花瓷.jpg` |
| 4. 序号.歌名 - 歌手 | `01.青花瓷 - 周杰伦.jpg` |

## 🔧 技术亮点

### 🌟 UTF-8中文支持
```cpp
// 完美处理中文文件名
std::string filename = "周杰伦 - 青花瓷.jpg";
// 使用Windows API宽字符转换确保兼容性
```

### 🛡️ 智能请求机制
- 随机User-Agent模拟真实浏览器
- 智能重试机制（失败自动重试）
- 人性化延迟避免请求过快
- 完整的HTTPS支持

### 🎯 精准解析算法
```cpp
// 多重正则表达式匹配确保解析成功率
std::vector<std::string> artist_patterns = {
    R"xxx(<meta property="og:music:artist" content="([^"]+)")xxx",
    R"xxx(<p class="des s-fc4">歌手：<span title="([^"]+)">)xxx",
    R"xxx(<div class="f-thide s-fc4"><span title="([^"]+)">)xxx"
};
```

## 📊 项目结构

```
netease_music_cover_downloader/
├── src/                    # 源代码目录
│   ├── main.cpp           # 主程序入口
│   ├── cover_downloader.cpp # 封面下载核心逻辑
│   └── utils.cpp          # 工具函数
├── include/               # 头文件目录
├── build/                 # 构建目录
├── covers/                # 下载的封面图片
├── CMakeLists.txt         # CMake构建配置
└── README.md             # 项目说明文档
```

## 🤝 贡献指南

欢迎各种形式的贡献！

### 🐛 报告问题
- 使用GitHub Issues报告bug
- 详细描述复现步骤
- 提供系统环境信息

### 💡 功能建议
- 提交Feature Request
- 参与讨论新功能
- 帮助完善文档

### 🎯 开发贡献
```bash
# Fork项目
# 创建特性分支
git checkout -b feature/AmazingFeature
# 提交更改
git commit -m 'Add some AmazingFeature'
# 推送到分支
git push origin feature/AmazingFeature
# 创建Pull Request
```

## 📜 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情

## 🙏 致谢

- 感谢网易云音乐提供精彩的音乐内容
- 感谢Boost库提供的强大网络支持
- 感谢所有测试和反馈的朋友们

## 📞 联系方式

- **GitHub**: [@yourusername](https://github.com/yourusername)
- **邮箱**: your.email@example.com
- **Issues**: [提交问题](https://github.com/yourusername/netease_music_cover_downloader/issues)

---

<p align="center">
  <strong>✨ 让音乐更有温度，让封面更有故事 ✨</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/github/stars/yourusername/netease_music_cover_downloader?style=social" alt="Stars">
  <img src="https://img.shields.io/github/forks/yourusername/netease_music_cover_downloader?style=social" alt="Forks">
  <img src="https://img.shields.io/github/watchers/yourusername/netease_music_cover_downloader?style=social" alt="Watchers">
</p>
