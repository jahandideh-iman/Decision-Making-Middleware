#include "CppUTest/TestHarness.h"
#include "DMEManager.h"
#include "DecisionTreeComponent.h"
#include "FiniteStateMachineComponent.h"
#include "DMEComponent.h"
#include <sstream>
#include "DMEUtilities.h"


class DMEComponentUpdateCounterMock : public DMEComponent
{
public:

	~DMEComponentUpdateCounterMock()
	{

	};
	void Update(float dt)
	{
		++updateCount;
	};

	int updateCount = 0;
};

class DMEComponentDeltaTimeSpy : public DMEComponent
{
public:

	~DMEComponentDeltaTimeSpy()
	{

	};

	void Update(float dt) override
	{
		lastDeltaTime = dt;
	};

	float lastDeltaTime = 0;
};

TEST_GROUP(DMEManager)
{
	DMEManager * manager;

	std::stringbuf* tempBuf = nullptr;
	std::istream* tempIStream = nullptr;

	void CallMultipleUpdate(unsigned numberOfTimes)
	{
		for (unsigned i = 0; i < numberOfTimes; ++i)
			manager->Update();
	}

	void setup()
	{
		manager = DMEManager::Get();
	}

	void teardown()
	{
		DMEManager::Destroy();
		SAFE_DELETE(tempBuf);
		SAFE_DELETE(tempIStream);
	}


	std::istream& CreateStream(string input)
	{
		tempBuf = new std::stringbuf(input);
		tempIStream = new std::istream(tempBuf);
		return  *(tempIStream);
	}
};

TEST(DMEManager, IsSingleton)
{
	POINTERS_EQUAL(manager, DMEManager::Get());
}

TEST(DMEManager, IsEmptyOnCreation)
{
	CHECK_TRUE(manager->isEmpty());
}

TEST(DMEManager, IsNotEmptyOnAddComponent)
{
	DecisionTreeComponent component;
	manager->AddComponent(&component);

	CHECK_FALSE(manager->isEmpty());
}

TEST(DMEManager, ComponentIsUpdatedOnEachUpdate)
{
	DMEComponentUpdateCounterMock componentMock;
	manager->AddComponent(&componentMock);

	CallMultipleUpdate(5);

	CHECK_EQUAL(5, componentMock.updateCount);
}

TEST(DMEManager, MultipleComponentsIsUpdateOnEachUpdate)
{
	DMEComponentUpdateCounterMock componentMock1;
	DMEComponentUpdateCounterMock componentMock2;
	manager->AddComponent(&componentMock1);
	manager->AddComponent(&componentMock2);

	CallMultipleUpdate(5);

	CHECK_EQUAL(5, componentMock1.updateCount);
	CHECK_EQUAL(5, componentMock2.updateCount);
}

TEST(DMEManager, DeltaTimeIsPropagatedToComponent)
{
	DMEComponentDeltaTimeSpy componentSpy;
	manager->AddComponent(&componentSpy);

	manager->Update(0.5);

	CHECK_EQUAL(0.5, componentSpy.lastDeltaTime);
}

TEST(DMEManager, CreateNullComponentOnEmptyInput)
{
	DMEComponent* comp = DMEManager::Get()->CreateComponent(CreateStream(""));

	POINTERS_EQUAL(nullptr, comp);
}

TEST(DMEManager, CreateDecisionTreeComponentOnValidInput)
{
	DMEComponent* comp = DMEManager::Get()->CreateComponent(CreateStream(
		"<DMEComponent  type=\"DecisionTree\" >"
		"</DMEComponent>"));

	DecisionTreeComponent* decisionTreeComp = dynamic_cast<DecisionTreeComponent*> (comp);

	CHECK_TRUE(decisionTreeComp != nullptr);

	delete comp;
}

TEST(DMEManager, CreateFiniteStateMachineComponentOnValidInput)
{
	DMEComponent* comp = DMEManager::Get()->CreateComponent(CreateStream(
		"<DMEComponent  type=\"FiniteStateMachine\" >"
		"</DMEComponent>"));

	FiniteStateMachineComponent* finiteStateMachineComp = dynamic_cast<FiniteStateMachineComponent*> (comp);

	CHECK_TRUE(finiteStateMachineComp != nullptr);

	delete comp;
}

