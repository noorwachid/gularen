#pragma once

#include "Gularen/Backend/Html/Composer.h"

namespace Gularen {
namespace Html {

class TemplateManager {
public:
	TemplateManager() {
	}

	void setDocument(Document* document) {
		_document = document;

		for (size_t j = 0; j < _document->annotations.size(); j += 1) {
			const Annotation& annotation = _document->annotations[j];
			_documentAnnotations[annotation.key] = annotation.value;
		}
	}

	void setTemplateFile(const std::string_view path) {
		std::ifstream file(path);
		_templateContent.assign(std::filesystem::file_size(path), '\0');
		file.read(_templateContent.data(), _templateContent.size());
	}

	std::string_view render() {
		_templateIndex = 0;

		while (_isBound(0)) {
			if (_get(0) == '<' && _isBound(4) &&
				_get(1) == '!' &&
				_get(2) == '-' &&
				_get(3) == '-' &&
				_get(4) == '[') {

				_advance(5);

				bool annotation = false;

				if (_isBound(0) && _get(0) == '[') {
					annotation = true;
					_advance(1);
				}

				size_t keyIndex = _templateIndex;

				while (_isBound(0) && _get(0) != ']') {
					_advance(1);
				}

				std::string_view key = std::string_view(_templateContent.data() + keyIndex, _templateIndex - keyIndex);

				if (_isBound(0) && _get(0) == ']') {
					_advance(1);
				}

				if (_isBound(0) && _get(0) == ']') {
					_advance(1);
				}

				if (_isBound(2) && _get(0) == '-' && _get(1) == '-' && _get(2) == '>') {
					_advance(3);
					if (annotation) {
						if (_documentAnnotations.count(key)) {
							std::string_view value = _documentAnnotations[key];
							_escape(value);
						}
					} else {
						if (key == "content") {
							Composer composer;
							std::string_view content = composer.compose(_document);
							_content.append(content.data(), content.size());
						} else if (key == "toc") {
							Composer composer;
							std::string_view content = composer.composeToc(_document);
							_content.append(content.data(), content.size());
						}
					}
				}

				continue;
			}

			_content.push_back(_get(0));
			_advance(1);
		}

		return std::string_view(_content.data(), _content.size());
	}

private:
	bool _isBound(size_t offset) {
		return _templateIndex + offset < _templateContent.size();
	}

	char _get(size_t offset) {
		return _templateContent[_templateIndex + offset];
	}

	void _advance(size_t offset) {
		_templateIndex += offset;
	}

	void _escape(std::string_view in) {
		for (size_t i = 0; i < in.size(); i += 1) {
			switch (in[i]) {
				case '<': _content.append("&lt;"); break;
				case '>': _content.append("&gt;"); break;
				case '&': _content.append("&amp;"); break;
				case '\"': _content.append("&quot;"); break;
				case '\'': _content.append("&#39;"); break;
				default: _content.append(1, in[i]); break;
			}
		}
	}

private:
	std::string _templateContent;

	size_t _templateIndex;

	std::unordered_map<std::string_view, std::string_view> _documentAnnotations;

	Document* _document;

	std::string _content;
};

}
}
