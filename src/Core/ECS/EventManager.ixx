export module EventManager;
import Globals;
import std;

namespace ECS {
	export class Event {
	public:
		virtual ~Event() = default;
		virtual EventType GetType() const = 0;
	};

	export using EventCallback = std::function<void(const Event&)>;

	export class EventManager {


	public:

		void Subscribe(EventType event_type, EventCallback event_callback)
		{
			event_listeners[event_type].push_back(event_callback);
		}

		void Emit(const std::shared_ptr<Event>& event)
		{
			event_queue.push(event);
		}

		void Dispatch()
		{
			while (!event_queue.empty())
			{
				auto& event = event_queue.front();
				EventType type = event->GetType();

				if (event_listeners.find(type) != event_listeners.end())
				{
					for (auto& callback : event_listeners[type])
					{
						callback(*event);
					}
				}
				event_queue.pop();
			}
		}

	private:
		std::unordered_map<EventType, std::vector<EventCallback>> event_listeners;
		std::queue<std::shared_ptr<Event>> event_queue;
	};
}