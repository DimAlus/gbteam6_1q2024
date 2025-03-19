#include "./GameModeDefault.h"
#include "GBTeam6/Game/GameInstanceDefault.h"
#include "GBTeam6/Player/PlayerPawnDefault.h"
#include "Delegates/DelegateSignatureImpl.inl"
#include <map>

UGameInstanceDefault *AGameModeDefault::GetGameInstanceDefault() {
	return Cast<UGameInstanceDefault>(GetGameInstance());
}

APlayerPawnDefault* AGameModeDefault::GetPlayerPawnDefault() {
	return Cast<APlayerPawnDefault>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AGameModeDefault::AddConsoleCommand(FName CommandName, FConsoleCommandBlueprintableSignature Callback) {
	if (!DT_ConsoleCommands) {
		return;
	}
	FTRConsoleCommandData* row = DT_ConsoleCommands->FindRow<FTRConsoleCommandData>(CommandName, "Command");
	if (row) {
		FConsoleCommandData data = (*row).Data;
		data.Callback = Callback;
		if (!data.CommandName.IsEmpty()) {
			CommandsData.Push(data);
		} else {
			UE_LOG(LgGame, Error, TEXT("<%s>: Can't initialize Console command <%s>: invalid data!"), *GetNameSafe(this), *CommandName.ToString());
		}
	} else {
		UE_LOG(LgGame, Error, TEXT("<%s>: Can't initialize Console command <%s>: not found at table!"), *GetNameSafe(this), *CommandName.ToString());
	}
}

void AGameModeDefault::InitializeConsoleCommands() {
	static std::map<EConsoleCommandArgumentType, void (*) (const FConsoleCommandArgumentInitializer&, const TArray<FString>&, TArray<FConsoleCommandParam>&, int&)> ArgumentParsers = {
		{ EConsoleCommandArgumentType::Int, [](const FConsoleCommandArgumentInitializer& initializer, const TArray<FString>& Args, TArray<FConsoleCommandParam>& Params, int& Index) {
			FConsoleCommandParam param;
			param.IntValue = initializer.IntDefault;
			if (!initializer.IsGhost && Index < Args.Num()) {
				const FString& val = Args[Index++];
				param.IntValue = FCString::Atoi(*val);
			}
			Params.Add(param);
		}},

		{ EConsoleCommandArgumentType::Float, [](const FConsoleCommandArgumentInitializer& initializer, const TArray<FString>& Args, TArray<FConsoleCommandParam>& Params, int& Index) {
			FConsoleCommandParam param;
			param.FloatValue = initializer.FloatDefault;
			if (!initializer.IsGhost && Index < Args.Num()) {
				const FString& val = Args[Index++];
				param.FloatValue = FCString::Atof(*val);
			}
			Params.Add(param);
		}},

		{ EConsoleCommandArgumentType::String, [](const FConsoleCommandArgumentInitializer& initializer, const TArray<FString>& Args, TArray<FConsoleCommandParam>& Params, int& Index) {
			FConsoleCommandParam param;
			param.StringValue = initializer.StringDefault;
			if (!initializer.IsGhost && Index < Args.Num()) {
				const FString& val = Args[Index++];
				param.StringValue = val;
			}
			Params.Add(param);
		}},

		{ EConsoleCommandArgumentType::Resource, [](const FConsoleCommandArgumentInitializer& initializer, const TArray<FString>& Args, TArray<FConsoleCommandParam>& Params, int& Index) {
			FConsoleCommandParam param;
			param.ResourceValue = initializer.ResourceDefault;
			if (!initializer.IsGhost && Index < Args.Num()) {
				TMap<FString, EResource> resmap = {
					{ "wood", EResource::Wood },
					{ "heal", EResource::Heal },
				};
				const FString& val = Args[Index++];
				if (resmap.Contains(val.ToLower())) {
					param.ResourceValue = resmap[val.ToLower()];
				}
			}
			Params.Add(param);
		}},

		{ EConsoleCommandArgumentType::Class, [](const FConsoleCommandArgumentInitializer& initializer, const TArray<FString>& Args, TArray<FConsoleCommandParam>& Params, int& Index) {
			FConsoleCommandParam param;
			param.ClassValue = initializer.ClassDefault;
			Params.Add(param);
		}},
	};


	for (const auto& iter : RegisteredConsoleCommands) {
		IConsoleManager::Get().UnregisterConsoleObject(iter.Value);
	}
	RegisteredConsoleCommands.Empty();
	for (const FConsoleCommandData& data : CommandsData) {

		RegisteredConsoleCommands.Add(data.CommandName, IConsoleManager::Get().RegisterConsoleCommand(
			*data.CommandName,
			*data.CommandDescription,
			FConsoleCommandWithArgsDelegate::CreateLambda([=](const TArray<FString>& Args) {
				TArray<FConsoleCommandParam> params;
				int Index = 0;
				for (const FConsoleCommandArgumentInitializer& init : data.Arguments) {
					if (ArgumentParsers.find(init.Type) != ArgumentParsers.end()) {
						ArgumentParsers[init.Type](init, Args, params, Index);
						params[params.Num() - 1].Type = init.Type;
					}
				}
				data.Callback.ExecuteIfBound(params);
			}),
			ECVF_Cheat
		));






		//FConsoleCommandWithArgsDelegate func = FConsoleCommandWithArgsDelegate::CreateUFunction(data.DelegateFloat.GetUObject(), data.DelegateFloat.GetFunctionName());
		//func.BindUFunction(data.DelegateFloat.GetUObject(), data.DelegateFloat.GetFunctionName());
		/*TObjectPtr<UObject> optr = this;
		FConsoleCommandWithArgsDelegate dlgt;
		bool found = true;*/

		/*switch (data.CommandType)
		{
		case EConsoleCommandType::Float:
			dlgt = FConsoleCommandWithArgsDelegate::CreateUFunction<UObject, float>(optr, data.DelegateFloat.GetFunctionName(), 2130.f);
			dlgt.ExecuteIfBound({ "1.2" });
			break;
		case EConsoleCommandType::Int:
			dlgt = FConsoleCommandWithArgsDelegate::CreateUFunction<UObject, int>(optr, data.DelegateInt.GetFunctionName(), 0);
			break;
		case EConsoleCommandType::StringArray:
			
			found = false;
			break;
		default:
			found = false;
			break;
		}
		if (found) {
			RegisteredConsoleCommands.Add(data.CommandName, IConsoleManager::Get().RegisterConsoleCommand(
				*data.CommandName,
				*data.CommandDescription,
				dlgt,
				ECVF_Cheat
			));

		}*/

		//IConsoleManager::Get().RegisterConsoleCommand(
		//	*data.CommandName,
		//	*data.CommandDescription,
		//	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([=](const TArray<FString>& Args, UWorld* World) {
		//		FTestSignature sig;
		//		switch (data.CommandType)
		//		{
		//		case EConsoleCommandType::Touch:
		//			sig.BindUFunction(data.DelegateFloat.GetUObject(), data.DelegateFloat.GetFunctionName());
		//			sig.Execute(Args);
		//			//data..ExecuteIfBound(Args);
		//			break;
		//		
		//		default:
		//			break;
		//		}
		//	}),
		//	ECVF_Cheat
		//);
	}
}
