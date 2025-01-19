import os
import sys
import re

component_name = sys.argv[1]

project_path = f"{os.path.dirname(os.path.abspath(__file__))}/.."
component_path = f"{project_path}/Source/GBTeam6/Component/{component_name}"
lib_path = f"{project_path}/Source/GBTeam6/Lib"
service_path = f"{project_path}/Source/GBTeam6/Service"
core_path = f"{project_path}/Source/GBTeam6/Interface"

enuming_tab_count = 3

try:
	os.mkdir(component_path)
except:
	print(f"Folder {component_path} already exists")

# BaseComponent.h
with open(f"{component_path}/{component_name}BaseComponent.h", "w", encoding="UTF-8") as f:
	f.write(f"""#pragma once

#include "CoreMinimal.h"
#include "GBTeam6/Component/BaseComponent.h"
#include "{component_name}BaseComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API U{component_name}BaseComponent : public UBaseComponent
{{
	GENERATED_BODY()

public:
	virtual void Initialize(const F{component_name}ComponentInitializer& Initializer);

	virtual void SaveComponent(F{component_name}SaveData& saveData);
	virtual void LoadComponent(const F{component_name}SaveData& saveData);

}};
""")

# BaseComponent.cpp
with open(f"{component_path}/{component_name}BaseComponent.cpp", "w", encoding="UTF-8") as f:
	f.write(f"""#include "./{component_name}BaseComponent.h"

void U{component_name}BaseComponent::Initialize(const F{component_name}ComponentInitializer& Initializer) {{
}}

void U{component_name}BaseComponent::SaveComponent(F{component_name}SaveData& saveData) {{
}}

void U{component_name}BaseComponent::LoadComponent(const F{component_name}SaveData& saveData) {{
}}
""")
	


# DefaultComponent.h
with open(f"{component_path}/{component_name}DefaultComponent.h", "w", encoding="UTF-8") as f:
	f.write(f"""#pragma once

#include "CoreMinimal.h"
#include "./{component_name}BaseComponent.h"
#include "{component_name}DefaultComponent.generated.h"

/**
 * 
 */
UCLASS()
class GBTEAM6_API U{component_name}DefaultComponent : public U{component_name}BaseComponent
{{
	GENERATED_BODY()
public:
	virtual void Initialize(const F{component_name}ComponentInitializer& Initializer) override;

	virtual void SaveComponent(F{component_name}SaveData& saveData) override;
	virtual void LoadComponent(const F{component_name}SaveData& saveData) override;

}};

""")

# DefaultComponent.cpp
with open(f"{component_path}/{component_name}DefaultComponent.cpp", "w", encoding="UTF-8") as f:
	f.write(f"""#include "./{component_name}DefaultComponent.h"


void U{component_name}DefaultComponent::Initialize(const F{component_name}ComponentInitializer& Initializer) {{
	Super::Initialize(Initializer);
}}

void U{component_name}DefaultComponent::SaveComponent(F{component_name}SaveData& saveData) {{
	Super::SaveComponent(saveData);
}}

void U{component_name}DefaultComponent::LoadComponent(const F{component_name}SaveData& saveData) {{
	Super::LoadComponent(saveData);
}}
""")
   
   
# Saving Structures
with open(f"{lib_path}/SavingStructures.h", "r", encoding="UTF-8") as f:
	data = f.read()

	data = re.sub(r"(?ms)([^\n]*?\n[^\n]*?FGameObjectSaveData.*?)\n+\};", f"""USTRUCT(BlueprintType)
struct F{component_name}SaveData {{
	GENERATED_BODY()
public:
}};


\\1

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	F{component_name}SaveData {component_name}Data{{}};
}};""", data, count=1)

with open(f"{lib_path}/SavingStructures.h", "w", encoding="UTF-8") as f:
	f.write(data)


# SaveService
with open(f"{service_path}/SaveService.cpp", "r", encoding="UTF-8") as f:
	data = f.read()

	data = re.sub(r"(?ms)(.*\n#include [^\n]*/Component/.*?\n)", f"""\\1#include "../Component/{component_name}/{component_name}BaseComponent.h"\n""", data, count=1)
	data = re.sub(
		r"""(?ms)(.*if \(auto \w*? = Cast<\w*>\(core->GetComponent\(EGameComponentType::\w*\)\)\).*?SaveComponent.*?\})""",
		f"""\\1
			if (auto {component_name.lower()} = Cast<U{component_name}BaseComponent>(core->GetComponent(EGameComponentType::{component_name}))) {{
				{component_name.lower()}->SaveComponent(SaveData.{component_name}Data);
			}}""",
		data,
		count=1,
	)
	data = re.sub(
		r"""(?ms)(.*if \(auto \w*? = Cast<\w*>\(core->GetComponent\(EGameComponentType::\w*\)\)\).*?LoadComponent.*?\})""",
		f"""\\1
	if (auto {component_name.lower()} = Cast<U{component_name}BaseComponent>(core->GetComponent(EGameComponentType::{component_name}))) {{
		{component_name.lower()}->LoadComponent(objectsSaveData.{component_name}Data);
	}}""",
		data,
		count=1,
	)

with open(f"{service_path}/SaveService.cpp", "w", encoding="UTF-8") as f:
	f.write(data)


# Enuming
with open(f"{lib_path}/Enuming.h", "r", encoding="UTF-8") as f:
	data = f.read()
    
	tabs = "\t" * max(1, enuming_tab_count - len(component_name) // 4)

	data = re.sub(r"(?ms)(EGameComponentType.*?)\n+\};", f"""\\1,\n\t{component_name}{tabs}UMETA(DisplayName = "{component_name}")\n}};""", data, count=1)

with open(f"{lib_path}/Enuming.h", "w", encoding="UTF-8") as f:
	f.write(data)


# Initializing
with open(f"{lib_path}/Initializing.h", "r", encoding="UTF-8") as f:
	data = f.read()
	
	data = re.sub(r"(?ms)(.*class \w*BaseComponent;)(.*?GBTEAM6_API UInitializing)", f"\\1\nclass U{component_name}BaseComponent;\\2", data, count=1)

	data = re.sub(r"(?ms)(.*)(/\*+/.*?FGameObjectInitializer)", f"""\\1/***********************************************************************************/
/// <summary>
/// {component_name} Initializing
/// </summary>
USTRUCT(BlueprintType)
struct F{component_name}ComponentInitializer {{
	GENERATED_BODY()
public:

}};

USTRUCT(BlueprintType)
struct F{component_name}ComponentInitData {{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<U{component_name}BaseComponent> ComponentClass{{}};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	F{component_name}ComponentInitializer ComponentInitializer{{}};
}};


\\2""", data, count=1)
	
	data = re.sub(
		r"(?ms)(FGameObjectInitData.*?)\n\};",
		f"""\\1

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	F{component_name}ComponentInitData {component_name}ComponentInitData{{}};
}};""",
		data,
		count=1,
	)
	
with open(f"{lib_path}/Initializing.h", "w", encoding="UTF-8") as f:
	f.write(data)


# GameObjectCore.cpp
with open(f"{core_path}/GameObjectCore.cpp", "r", encoding="UTF-8") as f:
	data = f.read()

	data = re.sub(r"(?ms)(.*\n#include [^\n]*/Component/.*?\n)", f"""\\1#include "../Component/{component_name}/{component_name}BaseComponent.h"\n""", data, count=1)

	data = re.sub(
		r"(?ms)(.*//\s*Create[^\n]*component.*?NewObject<U\w*BaseComponent>.*?BindComponent.*?\n)(.*New\w*Component->Initialize.*?\n)",
		f"""\\1
	//Create {component_name} component
	U{component_name}BaseComponent* New{component_name}Component = NewObject<U{component_name}BaseComponent>(
		owner,
		GetNvlClass(InitData.{component_name}ComponentInitData.ComponentClass, U{component_name}BaseComponent::StaticClass())
	);
	BindComponent(EGameComponentType::{component_name}, New{component_name}Component);
\\2	New{component_name}Component->Initialize(InitData.{component_name}ComponentInitData.ComponentInitializer);
""",
		data,
		count=1,
	)

with open(f"{core_path}/GameObjectCore.cpp", "w", encoding="UTF-8") as f:
	f.write(data)
