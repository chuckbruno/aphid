/*
 *  HFile.h
 *  mallard
 *
 *  Created by jian zhang on 10/10/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include <h5/HDocument.h>
#include "BaseFile.h"
#include "HBase.h"
#include <map>
namespace aphid {

class HFile : public BaseFile {
    HDocument::OpenMode m_openMode;
    
public:
	HFile();
	HFile(const char * name);
	virtual ~HFile();
	
	virtual bool doCreate(const std::string & fileName);
	virtual bool doRead(const std::string & fileName);
	virtual void doClose();
	
    void beginWrite();
	void flush();
	bool find(const std::string & pathName);
    bool isGroupOpened(const std::string & pathName) const;
    bool openGroup(const std::string & pathName);
    
    void setOpenMode(HDocument::OpenMode accessFlag);
    
protected:
	void useDocument();
	void setDocument(const HDocument & doc);
    bool entityExists(const std::string & name);
    
    template<typename Tb, typename Th>
    static bool LsNames2(std::vector<std::string> & dst, HBase * parent)
    {
        std::vector<std::string> aNames;
        parent->lsTypedChild<Tb>(aNames);
        std::vector<std::string>::const_iterator ita = aNames.begin();
        
        for(;ita!=aNames.end();++ita) {
            HBase child(*ita);
            LsNames2<Tb, Th>(dst, &child);
            child.close();
        }
        
        std::vector<std::string > hNames;
        parent->lsTypedChild<Th>(hNames);
        std::vector<std::string>::const_iterator itb = hNames.begin();
        
        for(;itb!=hNames.end();++itb)
            dst.push_back(*itb);
        
        return true;   
    }
private:
    void closeOpenedGroups();
private:
    std::map<std::string, HBase *> m_openedGroups;
	HDocument m_doc;
};

}