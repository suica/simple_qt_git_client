#include "libgit2pp.h"
GIT_BRANCHES GIT_REPO::branches() {
	return GIT_BRANCHES(*this);
}

GIT_CONFIG::GIT_CONFIG(GIT_REPO& r)
{
	if (git_repository_config(&_cfg, r._repo)) {
		throw GIT_EXCEPTION("can't open config for repo");
	}
	else {
	}
}

GIT_COMMITS::GIT_COMMITS(GIT_REPO& r)
	: _repo(r)
{
	git_revwalk_new(&_walk, _repo._repo);

	int error;
	git_object *obj;

	if ((error = git_revparse_single(&obj, _repo._repo, "HEAD")) < 0) {
		if (error == -3) {
			return;
		}
	}

	try {
		error = git_revwalk_push(_walk, git_object_id(obj));
	}
	catch (...)
	{

	}

	git_object_free(obj);
	if (error) throw error;

}

GIT_COMMIT GIT_COMMITS::GIT_COMMIT_WALKER::operator*()
{
	return GIT_COMMIT(_id, _c->_repo._repo);
}

GIT_CONFIG::ITEM GIT_CONFIG::ITER::operator*()
{
	return GIT_CONFIG::ITEM(_e, _i, _cfg);
}

static int resolve_refish(git_annotated_commit **commit, git_repository *repo, const char *refish)
{
	git_reference *ref;
	git_object *obj;
	int err = 0;

	assert(commit != NULL);

	err = git_reference_dwim(&ref, repo, refish);
	if (err == GIT_OK) {
		git_annotated_commit_from_ref(commit, repo, ref);
		git_reference_free(ref);
		return 0;
	}

	err = git_revparse_single(&obj, repo, refish);
	if (err == GIT_OK) {
		err = git_annotated_commit_lookup(commit, repo, git_object_id(obj));
		git_object_free(obj);
	}

	return err;
}

std::string GIT_REPO::branch_name() {

	git_reference *ref;
	char oid_hex[GIT_OID_HEXSZ + 1] = GIT_OID_HEX_ZERO;
	const char *refname;

	int error = git_reference_lookup(&ref, _repo, "HEAD");

	std::string out;
	switch (git_reference_type(ref)) {
	case GIT_REF_OID:
		git_oid_fmt(oid_hex, git_reference_target(ref));
		out = oid_hex;
		break;
	case GIT_REF_SYMBOLIC:
		out = git_reference_symbolic_target(ref);
		break;
	default:
		git_reference_free(ref);
		throw GIT_EXCEPTION("Unexpected reference type\n");
	}

	git_reference_free(ref);
	return out;
}

void GIT_REPO::checkout_branch(std::string const& refname)
{
	git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
	git_commit *target_commit = nullptr;
	git_annotated_commit *target = nullptr;

	opts.checkout_strategy = GIT_CHECKOUT_SAFE;

	if (resolve_refish(&target, _repo, refname.c_str())) {
		git_annotated_commit_free(target);

		throw GIT_EXCEPTION_CANT_FIND(refname);
	}
	else {
	}

	if (git_commit_lookup(&target_commit, _repo, git_annotated_commit_id(target))) {
		throw GIT_EXCEPTION("cant lookup commit for " + refname);
	}
	else if (git_checkout_tree(_repo, (const git_object *)target_commit, &opts)) {
		git_annotated_commit_free(target);
		throw GIT_EXCEPTION("error during checkout " + refname + ": " + std::string(giterr_last()->message));
	}
	else if (1) { //  } auto ref=git_annotated_commit_ref(target)) {
		if (git_repository_set_head(_repo, ("refs/heads/" + refname).c_str())) {
			git_annotated_commit_free(target);
			throw GIT_EXCEPTION("can't update HEAD to " + refname + ": "
				+ std::string(giterr_last()->message));
		}
		else {
		}
	}
	else {
		git_annotated_commit_free(target);
	}
}

std::string GIT_REPO::current_branch() {

	git_reference *ref = NULL;
	if (git_reference_lookup(&ref, _repo, "HEAD"))
		throw GIT_EXCEPTION("couldn't find HEAD");

	const char *refname = git_reference_name(ref);

	switch (git_reference_type(ref)) {
	case GIT_REF_OID:
	default:
		return refname;
	case GIT_REF_SYMBOLIC:
		return git_reference_symbolic_target(ref);
	}

}

std::ostream& operator<< (std::ostream& o, GIT_COMMIT const& c)
{
	return o;
}

std::ostream& operator<< (std::ostream& o, GIT_CONFIG::ITEM const& c)
{
	return o;
}

std::ostream& operator<< (std::ostream& o, GIT_BRANCH const& c)
{
	return o;
}
