#include "../../inc/HttpResponse.hpp"

std::pair<std::string, double> HttpResponse::extractLangAndQ(const std::string &langAndQ)
{
  std::string lang;
  double qValue = 1.0;

  std::size_t qPos = langAndQ.find(";q=");
  if (qPos != std::string::npos)
  {
    lang = langAndQ.substr(0, qPos);
    std::string qStr = langAndQ.substr(qPos + 3);
    try
    {
      qValue = atof(qStr.c_str());
    }
    catch (...)
    {
      qValue = 0.0;
    }
  }
  else
  {
    lang = langAndQ;
  }

  return std::make_pair(lang, qValue);
}

std::string HttpResponse::findBestLanguage(const std::vector<std::string> &matches, const std::vector<std::pair<std::string, double> > &langQPairs)
{
  std::string bestLanguage;
  /// @note rates lang preference
  // e.g Accept-Language: en-US,en;q=0.8,fr;q=0.7
  // en-US: No specified q value, defaults to 1.0 (highest).
  // en;q=0.8: English with a q value of 0.8.
  // fr;q=0.7: French with a q value of 0.7.
  double bestQValue = 0.0;

  for (std::vector<std::pair<std::string, double> >::const_iterator it = langQPairs.begin(); it != langQPairs.end(); ++it)
  {
    const std::string &lang = it->first;
    double qValue = it->second;

    if (lang == "*")
    {
      bestLanguage = matches.front();
      bestQValue = qValue;
      break; // Use the first language as default
    }
    else
    {
      for (std::vector<std::string>::const_iterator matchIt = matches.begin(); matchIt != matches.end(); ++matchIt)
      {
        if (matchIt->find("." + lang) != std::string::npos && qValue > bestQValue)
        {
          bestLanguage = *matchIt;
          bestQValue = qValue;
          break; // Found a better match, no need to continue
        }
      }
    }
  }

  return bestLanguage;
}

bool HttpResponse::localization(std::vector<std::string> &matches)
{
  std::string all = config_.getHeader("Accept-Language");

  std::vector<std::pair<std::string, double> > langQPairs;
  std::size_t pos = 0;
  while (pos < all.length())
  {
    std::size_t nextComma = all.find(',', pos);
    std::string langAndQ = all.substr(pos, (nextComma == std::string::npos) ? std::string::npos : nextComma - pos);
    langQPairs.push_back(extractLangAndQ(langAndQ));

    if (nextComma == std::string::npos)
      break;
    else
      pos = nextComma + 1;
  }

  std::string bestLanguage = findBestLanguage(matches, langQPairs);

  if (bestLanguage.empty())
    bestLanguage = "en"; // Default to English

  if (!bestLanguage.empty())
  {
    matches.clear();
    matches.push_back(bestLanguage);
    headers_["Content-Language"] = bestLanguage;
    return true;
  }

  return false;
}

CharsetAndQ HttpResponse::extractCharsetAndQ(const std::string &charsetAndQ)
{
  CharsetAndQ entry;
  entry.qValue = 1.0;

  size_t qPos = charsetAndQ.find(";q=");
  if (qPos != std::string::npos)
  {
    entry.charset = charsetAndQ.substr(0, qPos);
    std::string qStr = charsetAndQ.substr(qPos + 3);
    try
    {
      entry.qValue = atof(qStr.c_str());
    }
    catch (...)
    {
      entry.qValue = 0.0;
    }
  }
  else
  {
    entry.charset = charsetAndQ;
  }

  return entry;
}

std::string HttpResponse::findBestCharset(const std::vector<CharsetAndQ> &charsetAndQValues, const std::vector<std::string> &matches)
{
  std::string bestCharset;
  double bestQValue = 0.0;

  for (size_t i = 0; i < charsetAndQValues.size(); ++i)
  {
    CharsetAndQ entry = charsetAndQValues[i];
    if (entry.charset == "*")
    {
      bestCharset = matches.front();
      bestQValue = entry.qValue;
      break;
    }
    else
    {
      for (size_t j = 0; j < matches.size(); ++j)
      {
        if (matches[j].find("." + entry.charset) != std::string::npos && entry.qValue > bestQValue)
        {
          bestCharset = matches[j];
          bestQValue = entry.qValue;
          break;
        }
      }
    }
  }

  return bestCharset;
}

std::string HttpResponse::handleDefaultCharset(const std::string &bestCharset)
{
  return (bestCharset.empty()) ? "utf-8" : bestCharset;
}

void HttpResponse::handleAcceptCharset(std::vector<std::string> &matches)
{
  if (!config_.getHeader("Accept-Charset").empty())
  {
    charset_ = accept_charset(matches);
    file_->set_path(file_->getFilePath().substr(0, file_->getFilePath().find_last_of("/") + 1) + matches.front(), true);
  }
}

std::string HttpResponse::accept_charset(std::vector<std::string> &matches)
{
  std::string all = config_.getHeader("Accept-Charset");
  std::vector<CharsetAndQ> charsetAndQValues;

  size_t pos = 0;
  while (pos < all.length())
  {
    size_t nextComma = all.find(',', pos);
    size_t end = (nextComma == std::string::npos) ? all.length() : nextComma;
    std::string charsetAndQ = all.substr(pos, end - pos);

    CharsetAndQ entry = extractCharsetAndQ(charsetAndQ);
    charsetAndQValues.push_back(entry);

    if (nextComma == std::string::npos)
    {
      break;
    }
    else
    {
      pos = nextComma + 1;
    }
  }

  std::string bestCharset = findBestCharset(charsetAndQValues, matches);

  return handleDefaultCharset(bestCharset);
}


