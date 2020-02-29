import React from 'react'

export default f => {
	const isDesktop = () => {
		let desktopQuery = window.matchMedia('(min-width: 600px)')
		return desktopQuery.matches
	}

	const multiplier = isDesktop() ? 2 : 1
	const minWidth = isDesktop() ? 20 : 10
	const minScore = 10
	const width = f.score > minScore ? f.score * multiplier : minWidth
	const classes = f.score < minScore ? ' low-score' : ''

	console.log(multiplier)

	return (
		<li>
			<p>{f.text}</p>
			<div
				style={{ width: width + '%' }}
				className={'score-badge' + classes}
			>
				{f.score}%
			</div>
			<div className="data">
				Seen: {f.shown} | Votes: {f.votes}
			</div>
		</li>
	)
}
