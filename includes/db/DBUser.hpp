#ifndef USER_HPP
#define USER_HPP

#include <db/DBHistogram.hpp>
#include <db/DBPattern.hpp>
#include <vector>

using namespace std;
using namespace cv;

namespace Anakin {

/**
 * Describes a user in the database
 *
 * a user have an unique id (int)
 * and can have associated patterns, histograms and landscapes
 *
 * all addX functions will set the user id to X
 * where X can be a pattern, histogram, or label
 */
class DBUser {
public:
	/**
	 * Constructor
	 * id : the user's id
	 */
	DBUser(int id);
	~DBUser();
	int getID() const;

	Ptr<vector<Ptr<DBPattern>>> getPatterns() const;
	void addPattern(const Ptr<DBPattern> & p);

	Ptr<vector<Ptr<DBHistogram>>> getHistograms() const;
	void addHistogram(const Ptr<DBHistogram> & h);

	Ptr<vector<Ptr<DBHistogram>>> getLandscapes() const;
	void addLandscape(const Ptr<DBHistogram> & l);
protected:
private:
	int id;
	Ptr<vector<Ptr<DBPattern>>> patterns;
	Ptr<vector<Ptr<DBHistogram>>> histograms;
	Ptr<vector<Ptr<DBHistogram>>> landscapes;
};
}
;

#endif // USER_HPP
