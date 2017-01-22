// Normal attributes
	int n = shapes[shape_id].mesh.positions.size() / VALS_PER_VERT;
	vector<float>& normals = shapes[shape_id].mesh.normals;

#ifdef AUTO_NORMAL
	normals.clear();
#endif
	if (normals.size() != n*VALS_PER_NORMAL) {
		cout << "Shape_id: " << shape_id << " - Auto generated normal" << endl;
		int m = shapes[shape_id].mesh.indices.size();
		vector<float>& positions = shapes[shape_id].mesh.positions;
		vector<unsigned int>& indices = shapes[shape_id].mesh.indices;
		normals = vector<float> (n * VALS_PER_NORMAL, 0.f);
		vector<int> count (n, 0);
		for (int index=0; index<m;) {
			int p[3] = {indices[index], indices[index+1], indices[index+2]};
			glm::vec3 v[3];

			for (int i=0; i!=3; i++) {
				v[i] = glm::vec3(positions[3*p[i]], positions[3*p[i]+1], positions[3*p[i]+2]);
			}
			glm::vec3 normal = glm::normalize(glm::cross((v[1]-v[0]),(v[2]-v[0])));

			for (int i=0; i!=3; i++) {
				normals[3*p[i]] += normal[0];
				normals[3*p[i]+1] += normal[1];
				normals[3*p[i]+2] += normal[2];
				count[p[i]]++;
			}


			index += 3;
		}
		for (int i=0; i!=count.size(); i++) {
			assert(count[i] != 0);
			normals[3*i] /= count[i];
			normals[3*i+1] /= count[i];
			normals[3*i+2] /= count[i];
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(float) * shapes[shape_id].mesh.normals.size(),
				 &shapes[shape_id].mesh.normals[0],
				 GL_STATIC_DRAW);
	glEnableVertexAttribArray(normLoc);
	glVertexAttribPointer(normLoc, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);