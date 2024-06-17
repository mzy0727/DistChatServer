#pragma once

enum EnMsgType{
    LOGIN,     // 登录
    LOGIN_ACK, // 登录响应

    LOGINOUT, // 注销

    REG,     // 注册
    REG_ACK, // 注册响应

    ONE_CHAT,     // 聊天
    ONE_CHAT_ACK, // 聊天响应
    ONE_CHAT_MSG, // 聊天消息

    ADD_FRIEND,     // 添加好友
    ADD_FRIEND_ACK, // 添加好友响应

    CREATE_GROUP,     // 创建群组
    CREATE_GROUP_ACK, // 创建群组响应

    ADD_GROUP,     // 加入群组
    ADD_GROUP_ACK, // 加入群组响应

    GROUP_CHAT,     // 群聊天
    GROUP_CHAT_ACK, // 群聊天响应
    GROUP_CHAT_MSG, // 群聊天消息

    GET_GROUPLIST,     // 获取用户群组列表
    GET_GROUPLIST_ACK, // 获取用户群组列表响应

    GET_FRIENDLIST,     // 获取用户好友列表
    GET_FRIENDLIST_ACK, // 获取用户好友列表响应

    GET_OFFLINEMSG,     // 获取用户离线消息列表
    GET_OFFLINEMSG_ACK, // 获取用户离线消息列表响应
};