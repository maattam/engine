//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "shaderdata.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <memory>

using namespace Engine;

struct ShaderParser;

struct ParserRule
{
    virtual QRegExp& match() = 0;

    // Returns false if the line should be discarded
    virtual bool parse(QString& line, ShaderParser& parser) = 0;
};

struct ShaderParser
{
    typedef std::shared_ptr<ParserRule> ParserRulePtr;
    typedef QList<ParserRulePtr> ParserRuleList;

    ShaderParser(QByteArray& result, QStringList& files, const ParserRuleList& rules)
        : result_(result), additionalFiles_(files), rules_(rules)
    {
    }

    QString fileName_;
    QStringList& additionalFiles_;
    QByteArray& result_;
    const ParserRuleList& rules_;

    bool parse(const QString& fileName);
};

struct IncludeRule : public ParserRule
{
    IncludeRule()
        : rxInclude_("^\\s*#include\\s*\"([^\"\']+)\"\\s*$")
    {
    }

    QRegExp rxInclude_;

    virtual QRegExp& match()
    {
        return rxInclude_;
    }

    virtual bool parse(QString& line, ShaderParser& parser)
    {
        QString fileRel = rxInclude_.cap(1);
        QString dir = QFileInfo(parser.fileName_).dir().path();

        if(!dir.isEmpty())
        {
            dir += QDir::separator();
        }

        QByteArray result;
        if(!parser.parse(dir + fileRel))
        {
            return false;
        }

        line = result;
        parser.additionalFiles_ << dir + fileRel;

        return true;
    }
};

struct DefineRule : public ParserRule
{
    DefineRule(const ShaderData::DefineMap& map)
        : defines_(map), rxDefine_("^\\s*#define\\s*([\\d|\\w]+)\\s*<>\\s*$")
    {
    }

    const ShaderData::DefineMap& defines_;
    QRegExp rxDefine_;

    virtual QRegExp& match()
    {
        return rxDefine_;
    }

    // Returns false if the line should be discarded
    virtual bool parse(QString& line, ShaderParser& parser)
    {
        QString key = rxDefine_.cap(1);
        auto result = defines_.find(key);
        if(result != defines_.end())
        {
            line = line.replace("<>", result->toString());
        }

        else
        {
            qWarning() << __FUNCTION__ << "Named define" << key << "found, but no matching value exists.";
        }

        return true;
    }
};

ShaderData::ShaderData()
    : ResourceData()
{
}

bool ShaderData::load(const QString& fileName)
{
    ShaderParser::ParserRuleList rules;
    rules << std::make_shared<DefineRule>(defines_);
    rules << std::make_shared<IncludeRule>();

    ShaderParser parser(data_, additionalFiles_, rules);
    return parser.parse(fileName);
}

const QByteArray& ShaderData::data() const
{
    return data_;
}

QStringList ShaderData::queryFilesDebug() const
{
    return additionalFiles_;
}

void ShaderData::setDefines(const DefineMap& map)
{
    defines_ = map;
}

bool ShaderParser::parse(const QString& fileName)
{
    fileName_ = fileName;

    QFile file(fileName_);
    QString line;

    if(!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open" << fileName_ << file.errorString();
        return false;
    }
    
    while(!file.atEnd())
    {
        line = file.readLine();

        for(const ParserRulePtr& rule : rules_)
        {
            if(rule->match().exactMatch(line) && rule->parse(line, *this))
            {
                break;
            }
        }

        result_.append(line);
    }

    return true;
}