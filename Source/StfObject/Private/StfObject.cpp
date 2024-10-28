#include "StfObject.h"

void UStfObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}

bool UStfObject::IsSupportedForNetworking() const
{
	return true;
}

void UStfObject::Replicate()
{
	GetTypedOuter<AActor>()->AddReplicatedSubObject(this);
}

IMPLEMENT_MODULE(FDefaultModuleImpl, StfObject)