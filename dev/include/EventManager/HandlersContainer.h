#pragma once

#include <list>

namespace EventManager
{
	template <typename HandlerType_>
	class HandlersContainer
	{
	public:
		using HandlerType = HandlerType_;
		typedef std::list<HandlerType> container_type;

		class Token
		{
		public:
			using HandlersContainerType = HandlersContainer<HandlerType>;

			Token(typename HandlersContainerType::container_type::iterator it) : it_(it) {}

			typename HandlersContainerType::container_type::iterator it_;
		};

		Token PushBack(const HandlerType & handler) {
			return Token(handlers_.insert(handlers_.end(), handler));
		}

		void Remove(Token token) {
			handlers_.erase(token.it_);
		}

		template <typename... Args>
		void TriggerAll(const Args&... args) {
			for (auto& handler : handlers_) {
				handler.Handle(args...);
			}
		}

	private:
		container_type handlers_;
	};
}