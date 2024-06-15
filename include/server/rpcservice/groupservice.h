#pragma once

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "redis.h"
#include "GroupModel.h"
#include "FriendModel.h"
#include "UserModel.h"
#include "OfflineMsgModel.h"
#include "DistributedService.pb.h"
using namespace muduo;
using namespace muduo::net;
using namespace std;

class GroupService : public fixbug::GroupServiceRpc
{
public:
    std::vector<Group> GetGroupList(int userid)
    {
        vector<Group> groupuserVec =  _groupModel.queryGroups(userid);
        return groupuserVec;
    }

    void GetGroupList(::google::protobuf::RpcController *controller,
                      const ::fixbug::GetGroupListRequest *request,
                      ::fixbug::GetGroupListResponse *response,
                      ::google::protobuf::Closure *done)
    {
        int userid = request->userid();

        std::vector<Group> groupList = GetGroupList(userid);

        response->mutable_result()->set_success(true);
        response->mutable_result()->set_errmsg("");
        for (auto group_ : groupList)
        {
            auto *p = response->mutable_grouplist()->add_group();
            fixbug::Group grouptemp;
            grouptemp.set_groupid(group_.getId());
            grouptemp.set_groupname(group_.getName());
            grouptemp.set_groupdesc(group_.getDesc());
            for (auto &member : group_.getUsers())
            {
                fixbug::User usertemp;
                
                usertemp.set_userid(member.getId());
                usertemp.set_username(member.getName());
                usertemp.set_userstate(member.getState());
                *grouptemp.add_groupmember() = usertemp;
              //  *grouptemp.add_memberidentity() = member.second;
            }
            *p = grouptemp;
        }

        done->Run();
    }

    bool AddGroup(int userid, int groupid)
    {
       _groupModel.addGroup(userid, groupid, "normal");
        return true;
    }

    void AddGroup(::google::protobuf::RpcController *controller,
                  const ::fixbug::AddGroupRequest *request,
                  ::fixbug::AddGroupResponse *response,
                  ::google::protobuf::Closure *done)
    {
        int userid = request->userid();
        int groupid = request->groupid();

        bool ret = AddGroup(userid, groupid);

        if (ret == false)
        {
            response->mutable_result()->set_errmsg("加入群组失败");
        }
        else
        {
            response->mutable_result()->set_errmsg("");
        }
        response->mutable_result()->set_success(ret);

        done->Run();
    }

    GroupInfo CreateGroup(int userid, string groupname, string groupdesc)
    {
        Group group(-1, groupname, groupdesc);
        GroupInfo res;
        if ((res = _groupModel.createGroup(group)).success)
        {
            // 存储群组创建人信息
           _groupModel.addGroup(userid, group.getId(), "creator");
        }
        return res;
    }

    void CreateGroup(::google::protobuf::RpcController *controller,
                     const ::fixbug::CreateGroupRequest *request,
                     ::fixbug::CreateGroupResponse *response,
                     ::google::protobuf::Closure *done)
    {
        int userid = request->userid();
        string groupname = request->groupname();
        string groupdesc = request->groupdesc();

        GroupInfo ret = CreateGroup(userid, groupname, groupdesc);

        if (ret.success  == false)
        {
            response->mutable_result()->set_errmsg("创建群组失败");
        }
        else
        {
            response->mutable_result()->set_errmsg("");
        }
        response->mutable_result()->set_success(ret.success);
       // *response->mutable_result()->s = ret.groupid;
        
        done->Run();
    }
private:
     GroupModel _groupModel;
};