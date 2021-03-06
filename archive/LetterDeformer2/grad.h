#pragma once

// This file is part of libigl, a simple c++ geometry processing library.
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>

#include <Eigen/Geometry>
#include <vector>

template <typename T, typename S>
void igl_grad(const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &V,
	const Eigen::Matrix<S, Eigen::Dynamic, Eigen::Dynamic> &F,
	Eigen::SparseMatrix<T> &G)
{
	Eigen::PlainObjectBase<Eigen::Matrix<T, Eigen::Dynamic, 3> > eperp21, eperp13;
	eperp21.resize(F.rows(), 3);
	eperp13.resize(F.rows(), 3);

	for (int i = 0; i < F.rows(); ++i)
	{
		// renaming indices of vertices of triangles for convenience
		int i1 = F(i, 0);
		int i2 = F(i, 1);
		int i3 = F(i, 2);

		// #F x 3 matrices of triangle edge vectors, named after opposite vertices
		Eigen::Matrix<T, 1, 3> v32 = V.row(i3) - V.row(i2);
		Eigen::Matrix<T, 1, 3> v13 = V.row(i1) - V.row(i3);
		Eigen::Matrix<T, 1, 3> v21 = V.row(i2) - V.row(i1);

		// area of parallelogram is twice area of triangle
		// area of parallelogram is || v1 x v2 || 
		Eigen::Matrix<T, 1, 3> n = v32.cross(v13);

		// This does correct l2 norm of rows, so that it contains #F list of twice
		// triangle areas
		double dblA = std::sqrt(n.dot(n));

		// now normalize normals to get unit normals
		Eigen::Matrix<T, 1, 3> u = n / dblA;

		// rotate each vector 90 degrees around normal
		double norm21 = std::sqrt(v21.dot(v21));
		double norm13 = std::sqrt(v13.dot(v13));
		eperp21.row(i) = u.cross(v21);
		eperp21.row(i) = eperp21.row(i) / std::sqrt(eperp21.row(i).dot(eperp21.row(i)));
		eperp21.row(i) *= norm21 / dblA;
		eperp13.row(i) = u.cross(v13);
		eperp13.row(i) = eperp13.row(i) / std::sqrt(eperp13.row(i).dot(eperp13.row(i)));
		eperp13.row(i) *= norm13 / dblA;
	}

	std::vector<int> rs;
	rs.reserve(F.rows() * 4 * 3);
	std::vector<int> cs;
	cs.reserve(F.rows() * 4 * 3);
	std::vector<double> vs;
	vs.reserve(F.rows() * 4 * 3);

	// row indices
	for (int r = 0; r < 3; r++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int i = r*F.rows(); i < (r + 1)*F.rows(); i++) rs.push_back(i);
		}
	}

	// column indices
	for (int r = 0; r < 3; r++)
	{
		for (int i = 0; i < F.rows(); i++) cs.push_back(F(i, 1));
		for (int i = 0; i < F.rows(); i++) cs.push_back(F(i, 0));
		for (int i = 0; i < F.rows(); i++) cs.push_back(F(i, 2));
		for (int i = 0; i < F.rows(); i++) cs.push_back(F(i, 0));
	}

	// values
	for (int i = 0; i < F.rows(); i++) vs.push_back(eperp13(i, 0));
	for (int i = 0; i < F.rows(); i++) vs.push_back(-eperp13(i, 0));
	for (int i = 0; i < F.rows(); i++) vs.push_back(eperp21(i, 0));
	for (int i = 0; i < F.rows(); i++) vs.push_back(-eperp21(i, 0));
	for (int i = 0; i < F.rows(); i++) vs.push_back(eperp13(i, 1));
	for (int i = 0; i < F.rows(); i++) vs.push_back(-eperp13(i, 1));
	for (int i = 0; i < F.rows(); i++) vs.push_back(eperp21(i, 1));
	for (int i = 0; i < F.rows(); i++) vs.push_back(-eperp21(i, 1));
	for (int i = 0; i < F.rows(); i++) vs.push_back(eperp13(i, 2));
	for (int i = 0; i < F.rows(); i++) vs.push_back(-eperp13(i, 2));
	for (int i = 0; i < F.rows(); i++) vs.push_back(eperp21(i, 2));
	for (int i = 0; i < F.rows(); i++) vs.push_back(-eperp21(i, 2));

	// create sparse gradient operator matrix
	G.resize(3 * F.rows(), V.rows());
	std::vector<Eigen::Triplet<T> > triplets;
	for (int i = 0; i < (int)vs.size(); ++i)
	{
		triplets.push_back(Eigen::Triplet<T>(rs[i], cs[i], vs[i]));
	}
	G.setFromTriplets(triplets.begin(), triplets.end());
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template specialization
template void igl_grad<double, int>(Eigen::Matrix<double, -1, -1, 0, -1,-1> const&, Eigen::Matrix<int, -1, -1, 0, -1, -1> const&,Eigen::SparseMatrix<double, 0, int>&);
#endif

surface_mesh::Surface_mesh::Face_property<Vector3> meshGradFrom(surface_mesh::Surface_mesh * mesh, std::string property, std::string fgname, bool isNoramlize = true)
{
	Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> V(mesh->n_vertices(), 3);
	Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic> F(mesh->n_faces(), 3);
	Eigen::SparseMatrix<double> G;

	// Vertices
	auto points = mesh->vertex_property<Vector3>("v:point");
	for (auto v : mesh->vertices()) V.row(v.idx()) = points[v];

	// Faces
	for (auto f : mesh->faces())
	{
		std::vector<surface_mesh::Surface_mesh::Vertex> verts;
		for (auto v : mesh->vertices(f)) verts.push_back(v);
		F.row(f.idx()) = Eigen::Vector3i(verts[0].idx(), verts[1].idx(), verts[2].idx());
	}

	// Compute gradient 'G'
	igl_grad(V, F, G);

	// Load scalar function 'U'
	auto u = mesh->vertex_property<double>(property);
	VectorXd U(mesh->n_vertices());
	for (auto v : mesh->vertices()) U(v.idx()) = u[v];

	// Compute gradient of U
	MatrixXd GU = Map<const MatrixXd>((G*U).eval().data(), F.rows(), 3);

	// Compute gradient magnitude
	auto GU_mag = GU.rowwise().norm();

	surface_mesh::Surface_mesh::Face_property<Vector3> fg = mesh->face_property<Vector3>(fgname, Vector3(0,0,1));
	for (auto f : mesh->faces()) fg[f] = isNoramlize ? GU.row(f.idx()).normalized() : GU.row(f.idx());
	return fg;
}
