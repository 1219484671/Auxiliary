﻿#include "juexing.h"

juexing::juexing(HWND hwnd,int mode,int threadid)
{
    handle=hwnd;
    mode_type=mode;
    id=threadid;
}

void juexing::startTask()
{
    counter *countermanager=new counter();
    control *controller=new control(handle);
    match *matchmanager=new match(handle,controller);
    operations *operationsmanager=new operations(handle,controller,matchmanager,countermanager);
    countermanager->setProps(guiInfo.propcount);
    countermanager->setJieJieProps(guiInfo.jiejiepropcount);
    if(mode_type==team_mode)
    {
        is_leader_thread=matchmanager->matchLeader();
    }
    while(runflag)
    {
        countermanager->decreaseProps();
        if(countermanager->getProps()<0)
        {
            emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"已达设定樱饼上限",INFO);
            emit promptmanager->finishevent();
            delete matchmanager;
            delete controller;
            delete countermanager;
            return;
        }
        else
        {
            if(is_update_ui_thread)
            {
                emit promptmanager->updateUI(PROPS_TYPE,countermanager->getProps(),C_JUEXING);
            }
            emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"剩余樱饼数量"+QString::number(countermanager->getProps()),INFO);
        }
        if(mode_type==single_mode)
        {
            if(matchmanager->matchStrength(single_mode))
            {
                controller->click(posInfo.common.single_challenge_pos.xpos,posInfo.common.single_challenge_pos.ypos);
            }
            else
            {
                emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"未检测到副本"+guiInfo.floortext+"界面!",ERR);
                emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"停止!",ERR);
                emit promptmanager->errorevent();
            }
        }
        else if(mode_type==yyh_mode)
        {
            if(matchmanager->matchProp(yyh_mode))
            {
                controller->click(posInfo.common.single_challenge_pos.xpos,posInfo.common.single_challenge_pos.ypos);
            }
            else
            {
                emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"未检测到副本"+guiInfo.floortext+"界面!",ERR);
                emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"停止!",ERR);
                emit promptmanager->errorevent();
            }
        }
        else if(mode_type==team_mode)
        {
            if(is_leader_thread)
            {
                if(matchmanager->matchTemplateAndGetValue("title.png",matchmanager->getScreenshot(0,0.25,0,0.15))>0.9)
                {
                    delayer->delayms(&runflag,2000);
                    int time=0;
                    while(runflag && matchmanager->checkPersonCount()!=guiInfo.person)
                    {
                        delayer->delayms(&runflag,SAMPLE_TIME);
                        time++;
                        if(time==600)
                        {
                            emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"组队超时!",ERR);
                            emit promptmanager->errorevent();
                            return;
                        }
                    }
                    if(matchmanager->matchStrength(team_mode))
                    {
                        controller->click(posInfo.common.team_challenge_pos.xpos,posInfo.common.team_challenge_pos.ypos);
                    }
                }
                else
                {
                    emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"未检测到副本"+guiInfo.floortext+"界面!",ERR);
                    emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"停止!",ERR);
                    emit promptmanager->errorevent();
                }
            }
        }
        if(operationsmanager->fightOperations(&runflag) && !guiInfo.failstop)
        {

            emit promptmanager->failevent();
            delete matchmanager;
            delete operationsmanager;
            delete controller;
            delete countermanager;
            return;
        }
        if(guiInfo.stopwhenlimit)
        {
            if(countermanager->getJieJieProp()>=30)
            {
                emit promptmanager->updateShowArea("窗口"+QString::number(id)+":"+"结界突破券达到上限!已停止!",INFO);
                emit promptmanager->finishevent();
                delete matchmanager;
                delete operationsmanager;
                delete controller;
                delete countermanager;
                return;
            }
        }
        if(mode_type==single_mode)
        {
            while(runflag && !matchmanager->matchStrength(single_mode))
            {
                delayer->delayms(&runflag,SAMPLE_TIME);
                if(!guiInfo.failstop)
                {
                    controller->click(posInfo.common.finish_pos.xpos,posInfo.common.finish_pos.ypos);
                }
            }
        }
        else if(mode_type==team_mode)
        {
            while(runflag && matchmanager->matchTemplateAndGetValue("title.png",matchmanager->getScreenshot(0,0.25,0,0.15))<0.9)
            {
                delayer->delayms(&runflag,SAMPLE_TIME);
                if(!guiInfo.failstop)
                {
                    controller->click(posInfo.common.finish_pos.xpos,posInfo.common.finish_pos.ypos);
                }
            }
        }
        delayer->delayms(&runflag,1000);
    }
    delete matchmanager;
    delete controller;
    delete countermanager;
}

void juexing::run()
{
    runflag=true;
    startTask();
}
