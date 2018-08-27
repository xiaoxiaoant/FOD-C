@echo off

cd /d %~dp0

cd proto

..\protoc.exe --cpp_out=../src/ Qot_Sub.proto    Qot_UpdateBasicQot.proto  Common.proto Qot_UpdateBroker.proto  GetGlobalState.proto Qot_UpdateKL.proto  InitConnect.proto Qot_UpdateOrderBook.proto  KeepAlive.proto Qot_UpdateRT.proto  Notify.proto Qot_UpdateTicker.proto  Qot_Common.proto Trd_Common.proto  Qot_GetBasicQot.proto Trd_GetAccList.proto  Qot_GetBroker.proto Trd_GetFunds.proto  Qot_GetHistoryKL.proto Trd_GetHistoryOrderFillList.proto  Qot_GetHistoryKLPoints.proto Trd_GetHistoryOrderList.proto  Qot_GetKL.proto Trd_GetMaxTrdQtys.proto  Qot_GetOrderBook.proto Trd_GetOrderFillList.proto  Qot_GetPlateSecurity.proto Trd_GetOrderList.proto  Qot_GetPlateSet.proto Trd_GetPositionList.proto  Qot_GetReference.proto Trd_ModifyOrder.proto  Qot_GetRehab.proto Trd_Notify.proto  Qot_GetRT.proto Trd_PlaceOrder.proto  Qot_GetSecuritySnapshot.proto Trd_ReconfirmOrder.proto  Qot_GetStaticInfo.proto Trd_SubAccPush.proto  Qot_GetSubInfo.proto Trd_UnlockTrade.proto  Qot_GetSuspend.proto Trd_UpdateOrder.proto  Qot_GetTicker.proto Trd_UpdateOrderFill.proto  Qot_GetTradeDate.proto   Qot_RegQotPush.proto 

cd ..
