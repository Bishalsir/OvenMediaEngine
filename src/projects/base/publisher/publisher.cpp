#include "publisher_private.h"
#include "publisher.h"

Publisher::Publisher(const cfg::Host &host_info, const std::shared_ptr<MediaRouteInterface> &router)
	: _host_info(host_info),
	  _router(router)
{
}

bool Publisher::Start()
{
	logti("Trying to start publisher %d", GetPublisherType());

	return true;
}

bool Publisher::Stop()
{
	auto it = _applications.begin();

	while(it != _applications.end())
	{
		auto application = it->second;

		_router->UnregisterObserverApp(application.get(), application);

		it = _applications.erase(it);
	}

	return true;
}

const cfg::Host& Publisher::GetHostInfo()
{
	return _host_info;
}

bool Publisher::CreateApplication(const info::Application &app_info)
{
	auto application = OnCreateApplication(app_info);

	// 생성한 Application을 Router와 연결하고 Start
	_router->RegisterObserverApp(application.get(), application);

	// Application Map에 보관
	_applications[application->GetId()] = application;

	return true;
}

std::shared_ptr<Application> Publisher::GetApplicationByName(ov::String app_name)
{
	for(auto const &x : _applications)
	{
		auto application = x.second;
		if(application->GetName() == app_name)
		{
			return application;
		}
	}

	return nullptr;
}

std::shared_ptr<Stream> Publisher::GetStream(ov::String app_name, ov::String stream_name)
{
	auto app = GetApplicationByName(std::move(app_name));

	if(app != nullptr)
	{
		return app->GetStream(std::move(stream_name));
	}

	return nullptr;
}

std::shared_ptr<Application> Publisher::GetApplicationById(info::application_id_t application_id)
{
	auto application = _applications.find(application_id);

	if(application == _applications.end())
	{
		return nullptr;
	}

	return application->second;
}

std::shared_ptr<Stream> Publisher::GetStream(info::application_id_t application_id, uint32_t stream_id)
{
	auto app = GetApplicationById(application_id);

	if(app != nullptr)
	{
		return app->GetStream(stream_id);
	}

	return nullptr;
}

