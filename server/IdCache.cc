#include <stdio.h>
#include <muduo/base/Logging.h>
#include <cppmysql/ResultSet.h>
#include "IdCache.h"

using namespace std;
using namespace cppmysql;

std::string IdCache::dbip_;
int IdCache::dbport_ = 0;
std::string IdCache::dbusr_;
std::string IdCache::dbpwd_;

CachedIds IdCache::getId(int bizType, int num)
{
	LOG_TRACE << "getId: " << num;
	CachedIds * ids = NULL;
	CachedIdsMap::iterator it = cachedIds_.find(bizType);
	if(cachedIds_.end() == it)
	{
		ids = &(cachedIds_[bizType] = CachedIds());
	}
	else
	{
		ids = &(it->second);
	}
	//
	if(ids->nextId_ == ids->maxId_)
	{
		refillCachedIds(*ids, bizType);
	}
	// recalculate return id number
	int avalIdsNum = static_cast<int>(ids->maxId_-ids->nextId_);
	int giveIdsNum = 0;
	if(num >= avalIdsNum)
	{
		giveIdsNum = avalIdsNum;
	}
	else
	{
		giveIdsNum = num;
	}
	//
	CachedIds retIds;
	retIds.nextId_ = ids->nextId_;
	retIds.maxId_ = ids->nextId_ + giveIdsNum;
	//
	ids->nextId_ += giveIdsNum;
	//
	return retIds;

}

void IdCache::refillCachedIds(CachedIds &ids, int bizType)
{
	LOG_TRACE << "refillCachedIds" ;
	//
	if(!conn_)
	{
		if(!resetConn())
		{
			return;
		}
	}
	//
	try
	{
		char sql[256] = {0};
		snprintf(sql, sizeof(sql), "select * from idtable where biz_type=%d for update", bizType);
		if(!conn_->query(sql))
		{
			LOG_ERROR << "query failed: " << conn_->getErrorStr() << ", sql=" << sql;
			throw 1;
		}
		ResultSetPtr rsp = conn_->getResultSet(true);
		//
		IdType bizId;
		int idStep;
		int rowCnt = 0;
		while(rsp->moveNext())
		{
			bizId = rsp->getUInteger("biz_id");
			idStep = static_cast<int>(rsp->getInteger("id_step"));
			++rowCnt;
		}
		if(0 == rowCnt)
		{
			LOG_ERROR << "query resultset is empty, sql = " << sql;
			throw 1;			
		}
		//
		snprintf(sql, sizeof(sql), "update idtable set biz_id=biz_id+id_step where biz_type=%d", bizType);
		if(!conn_->query(sql))
		{
			LOG_ERROR << "query failed: " << conn_->getErrorStr() << ", sql=" << sql;
			throw 1;
		}
		if(!conn_->commit())
		{
			LOG_ERROR << "commit failed: " << conn_->getErrorStr();
			throw 1;
		}
		ids.nextId_ = bizId;
		ids.maxId_ = bizId + idStep;
	}
	catch(int)
	{
		LOG_TRACE << "Rollback";
		if(!conn_->rollback())
		{
			LOG_ERROR << "rollback failed: " << conn_->getErrorStr();
			// reset connection
			resetConn();
		}
	}
	//
}

void IdCache::setDbInfo(const std::string &dbip, int dbport, const std::string &dbusr, const std::string &dbpwd)
{
	dbip_ = dbip;
	dbport_ = dbport;
	dbusr_ = dbusr;
	dbpwd_ = dbpwd;
}

bool IdCache::resetConn()
{
	conn_.reset(new Connection(dbip_.c_str(), dbport_, dbusr_.c_str(), dbpwd_.c_str(), "idserver"));
	if(!conn_->connect())
	{
		LOG_ERROR << "connect failed: " << conn_->getErrorStr();
		return false;
	}
	if(!conn_->setAutoCommit(false))
	{
		LOG_ERROR << "close autocommit failed." << conn_->getErrorStr();
		return false;
	}
	return true;
}

