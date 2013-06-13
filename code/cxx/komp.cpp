#include "komp.hpp"
#include "log.hpp"
#include <assert.h>

namespace komp {
	namespace mq
	{
		ctx::ctx()
		{
			thisCheckpoint();
		}
		
		ctx::~ctx()
		{
			thisCheckpoint();
		}
		
		ctx_thread * ctx::getThreadContext(std::thread::id threadID)
		{
			ctx_thread * ret;
			{
				std::unique_lock<std::mutex> lock(m_mThreadContext);
				assert(m_threadContexts.find(threadID) != m_threadContexts.end());
				ret = m_threadContexts[threadID];
			}
			return ret;
		}
		
		void ctx::attachThreadContext(std::thread::id threadID, ctx_thread *ctxThread)
		{
			std::unique_lock<std::mutex> lock(m_mThreadContext);
			assert(m_threadContexts.find(threadID) == m_threadContexts.end());
			m_threadContexts.emplace(std::make_pair(threadID, ctxThread));
		}
		
		void ctx::detachThreadContext(std::thread::id threadID)
		{
			bool bDelete = false;
			{
				std::unique_lock<std::mutex> lock(m_mThreadContext);
				m_threadContexts.erase(threadID);
				if (!m_threadContexts.size())
				{
					bDelete = true;
				}
			}
			if (bDelete)
			{
				delete this;
			}
		}
		
		
	}
}
namespace komp
{
	namespace mq
	{
		ctx_thread::ctx_thread():m_ctx(new ctx()), m_threadID(getCurrentThreadID())
		{
			//thisCheckpoint();
			m_ctx->attachThreadContext(m_threadID, this);
		}
		ctx_thread::ctx_thread(ctx * ctx): m_ctx(ctx), m_threadID(getCurrentThreadID())
		{
			//thisCheckpoint();
			m_ctx->attachThreadContext(m_threadID, this);
		}
		
		ctx_thread::~ctx_thread()
		{
			//thisCheckpoint();
			m_ctx->detachThreadContext(getCurrentThreadID());
		}
		
		std::thread::id ctx_thread::getCurrentThreadID()
		{
			return std::this_thread::get_id();
		}
		
		ktx * ctx_thread::attachThread()
		{
			return new ctx_thread(m_ctx);
		}
		
		
		qp_outer * ctx_thread::createQProcessor(qp_logic * logic)
		{
			return nullptr;
		}
	}
}