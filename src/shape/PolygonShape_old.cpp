// Backup old draw method sebelum diubah

//--------------------------------------------------------------
void PolygonShape::draw() const {
	ofPushStyle();
	ofFill();

	// Cek tipe animation dan apply effect yang sesuai
	if (animation) {
		ofColor animatedColor = fillColor;

		// Cek FadeInAnimation
		if (auto* fadeAnim = dynamic_cast<FadeInAnimation*>(animation.get())) {
			float progress = animation->getProgress();

			// Kalau progress masih 0, alpha harus 0 (BENAR-BENAR HIDDEN)
			if (progress == 0.0f) {
				animatedColor.a = 0;  // FORCE alpha 0!
			} else {
				// Progress > 0, pakai current alpha
				float currentAlpha = fadeAnim->getCurrentAlpha();
				animatedColor.a = static_cast<int>(currentAlpha);
			}

			ofSetColor(animatedColor);

			// Gambar polygon tanpa wobble
			ofBeginShape();
			for (auto& v : vertices) {
				ofVertex(v.x, v.y);
			}
			ofEndShape(true);
		}
		// Cek WobbleAnimation
		else if (auto* wobbleAnim = dynamic_cast<WobbleAnimation*>(animation.get())) {
			ofSetColor(animatedColor);

			// Gambar polygon dengan wobble effect pada vertices
			glm::vec2 offset = wobbleAnim->getWobbleOffset();

			ofBeginShape();
			for (auto& v : vertices) {
				// Setiap vertex diberi offset yang sama (seluruh polygon bergoyang)
				ofVertex(v.x + offset.x, v.y + offset.y);
			}
			ofEndShape(true);
		}
		// Cek FillAnimation
		else if (auto* fillAnim = dynamic_cast<FillAnimation*>(animation.get())) {
			// Wave fill shader code...
		}
		// Unknown animation type - fallback ke no animation
		else {
			ofSetColor(fillColor);
			ofBeginShape();
			for (auto& v : vertices) {
				ofVertex(v.x, v.y);
			}
			ofEndShape(true);
		}
	} else {
		// Tidak ada animation, pakai fillColor asli
		ofSetColor(fillColor);

		ofBeginShape();
		for (auto& v : vertices) {
			ofVertex(v.x, v.y);
		}
		ofEndShape(true);
	}

	ofPopStyle();
}
