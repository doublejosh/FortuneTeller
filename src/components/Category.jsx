import React from 'react'
import Fortune from './Fortune'

export default props => {
	const getScore = (votes, shown) => {
		if (shown === 0 || votes > shown) return 0
		return Math.round((parseInt(votes) / parseInt(shown)) * 100)
	}

	let totalShown = 0,
		totalVotes = 0

	return (
		<section className={'category ' + props.id}>
			<h3>{props.label}</h3>
			<ul className="fortunes">
				{props.fortunes &&
					props.fortunes
						.filter(f => f.category === props.id)
						.map(f => ({ ...f, score: getScore(f.votes, f.shown) }))
						.sort((a, b) => b.score - a.score)
						.map((f, i) => {
							totalShown += f.shown
							totalVotes += f.votes
							return <Fortune key={i} {...f} />
						})}
			</ul>
			<div className="count-badge">
				{totalVotes} / {totalShown}
				{' (' + Math.round((totalVotes / totalShown) * 100)}%)
			</div>
		</section>
	)
}
