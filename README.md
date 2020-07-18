# CameraRecord

Camera recording in Qt 4.7

## 更新日志 Update Log

### 7-13 master

1. 回放速度过快
2. 录制播放后，程序不可再用的问题

1. Replay speed is too fast
2. There is a problem that the program can no longer be used after recording and playback

### 7-14 master

1. 按钮逻辑和UI界面优化
2. 录制结束后，会自动回到预览状态

1. Button logic and UI interface optimization
2. After the recording is over, it will automatically return to the preview state

### 7-15 master

1. 添加了计时器，在录制时会计时，回放时显示录制长度
2. 录制结束后，再次点击回放可以再次回放

1. Added a timer, accounting during recording. Display recording length during playback.
2. After the recording is over, click "replay" to play back again

### 7-16 :arrow_heading_up: Qt5 OpenCV3.4

:warning: 这一版本不能直接在Qt5.9中运行，需要自己行编译搭建OpenCV3.4的运行环境！

1. 完成了Qt5.9版本加OpenCV4.3的迁移
2. 添加了人脸识别模块，可以用单选按钮开关模块
3. OpenCV版本与现Qt版本并不兼容，已新建分支保存 [Opencv分支](https://github.com/UESTC-ProjectM4/CameraRecord/tree/Qt5OpenCV3.4)

:warning:This version cannot be run directly in Qt5.9, you need to compile and build the operating environment of OpenCV3.4 by yourself!

1. Completed Qt5.9 version plus OpenCV4.3
2. Added face recognition module, you can switch the module with radio buttons
3. This version save in branch [Qt5OpenCV3.4](https://github.com/UESTC-ProjectM4/CameraRecord/tree/Qt5OpenCV3.4)

### TODO List

### 7-14 master

1. 添加录制时间显示 :heavy_check_mark:

2. 回放时对齐时间戳，不应由系统性能决定播放速度 :x:

   理由：保存文件时不存在时间戳，无法要求图片严格按照时间戳显示

### 7-15 master

1. 文件可以选择放在某个地方
2. 可以打开任意位置的回放文件
3. 显示存放文件夹的情况

### 7-16  Qt5 OpenCV3.4

1. 需要修改获取逻辑来控制录制和播放速度
2. 需要处理程序容易崩溃的问题