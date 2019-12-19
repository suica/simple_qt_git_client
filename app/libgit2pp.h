#pragma once

#include <git2.h>
#include <git2/repository.h>
#include <git2/annotated_commit.h>
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/revwalk.h>
#include <git2/revparse.h>
#include <git2/object.h>
#include <git2/commit.h>
#include <git2/branch.h>
#include <git2/config.h>
#include <git2/refs.h>
#include <git2/checkout.h>

#include <assert.h>

#include <string> 
#include <vector> 

class GIT_EXCEPTION : public std::exception {
public:
	explicit GIT_EXCEPTION(std::string const& s) : _what(s) {}
	~GIT_EXCEPTION() noexcept {}
public:
	virtual const char* what() const noexcept {
		return _what.c_str();
	}
private:
	std::string _what;
};
class GIT_EXCEPTION_CANT_FIND : public GIT_EXCEPTION {
public:
	explicit GIT_EXCEPTION_CANT_FIND(std::string const& s) :
		GIT_EXCEPTION("cant find " + s) {}
};
class GIT_EXCEPTION_INVALID : public GIT_EXCEPTION {
public:
	explicit GIT_EXCEPTION_INVALID(std::string const& s) :
		GIT_EXCEPTION("invalid " + s) {}
};

class GIT_SIGNATURE {
public:
	GIT_SIGNATURE(git_commit const* c) {
		assert(c);
		const git_signature *sig;
		if (!(sig = git_commit_author(c))) {
			throw GIT_EXCEPTION("something wrong in sig");
		}
		else {
			_s = std::make_pair(std::string(sig->name), std::string(sig->email));
		}
	}
public:
	std::string const& name() const { return _s.first; }
	std::string const& email() const { return _s.second; }
private:
	std::pair<std::string, std::string> _s;
};

class GIT_COMMIT {
public:
	explicit GIT_COMMIT(git_oid i, git_repository* r) : _id(i) {
		if (git_commit_lookup(&_c, r, &_id)) {
			throw "lookup error\n";
		}
		else {
		}
	}
    GIT_COMMIT(){}
public:
	bool operator==(GIT_COMMIT const& x) const {
		return git_oid_equal(&_id, &x._id);
	}
	bool operator!=(GIT_COMMIT const&x) {
		return !(*this == x);
	}

	std::string id() const {
		char buf[GIT_OID_HEXSZ + 1];
		git_oid_fmt(buf, &_id);
		buf[GIT_OID_HEXSZ] = '\0';
		return std::string(buf);
	}

	std::ostream& print(std::ostream& o) const {
		return o << id();
	}

	std::string author() const {
		return signature().name();
	}

	std::string message() {
		return git_commit_message(_c);
	}

	time_t time(unsigned len = 99) const {
		return git_commit_time(_c);
	}

	GIT_SIGNATURE signature() const {
		return GIT_SIGNATURE(_c);
	}
    git_oid getOid(){
        return _id;
    }
public:
    git_oid _id;
	git_commit* _c;
}; // GIT_COMMIT

class GIT_REPO;

class GIT_CONFIG {
public: // types	
	class ITEM {
	public:
		explicit ITEM(git_config_entry* e, git_config_iterator* p, GIT_CONFIG& c)
			: _cfg(c), _entry(e)
		{
		}
		explicit ITEM(GIT_CONFIG& p, std::string const& what)
			: _cfg(p) {
			if (git_config_get_entry(&_entry, p._cfg, what.c_str())) {
				throw GIT_EXCEPTION_CANT_FIND(what);
			}
			else {
			}
		}
		ITEM& operator=(const std::string& v) {
			if (!_cfg._cfg) {
			}
			else if (int error = git_config_set_string(_cfg._cfg, name().c_str(), v.c_str())) {
				throw GIT_EXCEPTION("can't set " + std::to_string(error));
			}

			return *this;
		}

		void erase() {
			if (int error = git_config_delete_entry(_cfg._cfg, name().c_str()))
				throw GIT_EXCEPTION("can't erase " + name() + " because " + std::to_string(error));
		}

		std::string value() const {
			assert(_entry);
			return _entry->value;
		}
		std::string name() const {
			assert(_entry);
			return _entry->name;
		}

		std::ostream& print(std::ostream& o) const {
            std::string www = " = ";
            return o << name() << www << value();
		}


	private:
		GIT_CONFIG& _cfg;
		git_config_entry* _entry;
	}; // ITEM

	class ITER {
	public:
		ITER(ITER const& i)
			: _cfg(i._cfg), _i(i._i), _e(i._e)
		{
		}
	public:
		explicit ITER(GIT_CONFIG& c) : _cfg(c) {
			if (git_config_iterator_new(&_i, c._cfg)) {
				throw GIT_EXCEPTION("iter error");
			}
			else {
			}

			operator++();
		}
		explicit ITER(GIT_CONFIG& c, int)
			: _cfg(c), _i(nullptr) {
		} // "end"

		~ITER() {
			git_config_iterator_free(_i);
		}

		ITER& operator++() {
			assert(_i); // not end.
			// need to keep a pointer to entry, as we can only fetch it once (?!)
			int res = git_config_next(&_e, _i);

			if (res == GIT_ITEROVER) {
				_i = nullptr;
				_e = nullptr;
				assert(*this == ITER(_cfg, 0));
			}
			return *this;
		}
		bool operator==(ITER const&x) const {
			return _i == x._i;
		}
		bool operator!=(ITER const&x) {
			return !(*this == x);
		}
		ITEM operator*();
	private:
		GIT_CONFIG& _cfg;
		git_config_iterator* _i;
		git_config_entry* _e;
	}; // ITER

public: // construct
	GIT_CONFIG(GIT_REPO& r);
	~GIT_CONFIG() {
	}

public:
	ITEM operator[](std::string const& s) {
		try {
			return ITEM(*this, s);
		}
		catch (GIT_EXCEPTION const&) {
			create(s);
			return ITEM(*this, s);
		}
	}
	// private:
    ITEM create(const std::string& what) {
        //else if (int error = git_config_set_string(_cfg._cfg, name().c_str(), v.c_str())) {
        if (git_config_set_string(_cfg, what.c_str(), "")) {
            throw "orz";
        } else {
        }
        return (*this)[what];
	}
public: // iterate
	ITER begin() {
		return ITER(*this);
	}
	ITER end() {
		return ITER(*this, 0);
	}

private:
	git_config * _cfg;
}; // GIT_CONFIG

class GIT_COMMITS {
public: // types	
	class GIT_COMMIT_WALKER {
	public:
		GIT_COMMIT_WALKER(GIT_COMMITS& c) : _c(&c) {
			std::fill((char*)&_id, (char*)(&_id) + sizeof(git_oid), 1);
			operator++();
		}
		GIT_COMMIT_WALKER() : _c(nullptr) {
			std::fill((char*)&_id, (char*)(&_id) + sizeof(git_oid), 0);
		} // "end"

		GIT_COMMIT_WALKER& operator++() {
			assert(!git_oid_iszero(&_id));
			if (git_revwalk_next(&_id, _c->_walk)) {
				std::fill((char*)&_id, (char*)(&_id) + sizeof(git_oid), 0);
			}
			else {
			}
			return *this;
		}
		bool operator==(GIT_COMMIT_WALKER const&x) const {
			return git_oid_equal(&_id, &x._id);
		}
		bool operator!=(GIT_COMMIT_WALKER const&x) {
			return !(*this == x);
		}
		GIT_COMMIT operator*();
	private:
		GIT_COMMITS* _c;
		git_oid _id;
	};

public: // construct
	GIT_COMMITS(GIT_REPO& r);
	~GIT_COMMITS() {
		git_revwalk_free(_walk);
	}
public: // iterate
	GIT_COMMIT_WALKER begin() {
		return GIT_COMMIT_WALKER(*this);
	}
	GIT_COMMIT_WALKER end() {
		return GIT_COMMIT_WALKER();
	}

private:
	GIT_REPO& _repo;
	git_revwalk* _walk;
};

class GIT_BRANCH {
public:
	explicit GIT_BRANCH(git_reference* ref) :_ref(ref) {}

public:
	std::string name() const {
		const char* c;
		if (_ref == NULL)
			return "none";
		git_branch_name(&c, _ref);
		return std::string(c);
	}
	std::ostream& print(std::ostream& o) const {
		return o << name();
	}

private:
	git_reference* _ref;
};

class GIT_BRANCHES;

// a git repository
class GIT_REPO {
public:
	enum class create_t {
		_create
	};

	GIT_REPO(create_t, std::string path = ".") {
		if (!git_repository_open(&_repo, path.c_str())) {
			throw GIT_EXCEPTION("already there. use the constructor without _create");
		}
		else if (git_repository_init(&_repo, path.c_str(), 0)) {
			throw GIT_EXCEPTION("internal error creating repo");
		}
	}

	GIT_REPO(std::string path = ".") : _repo(nullptr) {
		git_libgit2_init();

		int error = git_repository_open(&_repo, path.c_str());
		if (error < 0) {
			const git_error *e = giterr_last();
			throw GIT_EXCEPTION_CANT_FIND(
				" repository (" + std::string(e->message) + ", "
				+ "error " + std::to_string(error) + " " + std::to_string(e->klass) + ")");
		}

	}
	~GIT_REPO() {
		git_repository_free(_repo);
		git_libgit2_shutdown();
	}

	GIT_COMMITS commits() {
		return GIT_COMMITS(*this);
	}

	GIT_CONFIG config() {
		return GIT_CONFIG(*this);
	}

	GIT_BRANCHES branches();
	std::string branch_name();
	void checkout_branch(std::string const&);
	std::string current_branch();

	git_repository* _repo;

private:
public:
	friend class GIT_COMMITS;
	friend class GIT_CONFIG;
};

class GIT_BRANCHES {
public:
	class iterator {
	public:
		explicit iterator(GIT_BRANCHES& b) : _br(b), _e(nullptr) {
			if (git_branch_iterator_new(&_i, b._repo._repo, GIT_BRANCH_ALL)) {
				throw GIT_EXCEPTION("branch iter error");
			}
			else {
			}

			operator++();
		}
		explicit iterator(GIT_BRANCHES& b, int) : _br(b), _e(nullptr), _i(nullptr) {
		}

	public:
		bool operator==(iterator const&x) const {
			return  _i == x._i;
		}

		bool operator!=(iterator const&x) const {
			return _i != x._i;
		}

		GIT_BRANCH operator*() {
			return GIT_BRANCH(_e);
		}

		iterator& operator++() {
			// need to keep a pointer to entry, as we can only fetch it once (?!)
			int end=git_branch_next(&_e, &ot, _i);
			if (end) {
				_i = nullptr;
				_e = nullptr;
			}
			return *this;
		}

	private:
		git_branch_iterator* _i;
		GIT_BRANCHES& _br;
		git_reference* _e;
		git_branch_t ot;
	};

public:
	explicit GIT_BRANCHES(GIT_REPO& r) : _repo(r)
	{
	}

public:
	iterator begin() {
		return iterator(*this);
	}
	iterator end() {
		return iterator(*this, 0);
	}
	GIT_BRANCH create(std::string const& name) {
		git_reference* out;
		git_reference* r;
		if (git_repository_head(&r, _repo._repo)) {
			throw GIT_EXCEPTION("cant obtain reference to HEAD");
		}
		else {
		}
		const git_oid* h = git_reference_target(r);
		git_commit* c;
		if (git_commit_lookup(&c, _repo._repo, h)) {
			throw GIT_EXCEPTION("cannot lookup HEAD commit");
		}
		else {
		}
		if (git_branch_create(&out, _repo._repo, name.c_str(), c, 0/*force*/)) {
			free(c);
			throw GIT_EXCEPTION_INVALID(name);
		}
		else {
			free(c);
		}

		return GIT_BRANCH(out);
	}
	void erase(std::string const& name) {

		if (_repo.current_branch() == "refs/heads/" + name) // we already have that name, do nothing
			throw GIT_EXCEPTION("can't delete current branch");

		git_reference* r;

		int err = git_reference_dwim(&r, _repo._repo, name.c_str());
		if (err) {
			git_reference_free(r);
			throw GIT_EXCEPTION_CANT_FIND(name);
		}

		err = git_branch_delete(r);

		if (err)
			throw GIT_EXCEPTION("error deleting branch " + name);

		git_reference_free(r);
	}

private:
	GIT_REPO& _repo;
}; // GIT_BRANCHES

extern std::ostream& operator<< (std::ostream& o, GIT_COMMIT const& c);

extern std::ostream& operator<< (std::ostream& o, GIT_CONFIG::ITEM const& c);

extern std::ostream& operator<< (std::ostream& o, GIT_BRANCH const& c);
